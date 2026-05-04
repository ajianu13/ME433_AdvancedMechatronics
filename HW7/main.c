#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

#define CS_PIN 17

static inline void cs_select() {
    gpio_put(CS_PIN, 0);
    asm volatile("nop; nop; nop;");
}
static inline void cs_deselect() {
    asm volatile("nop; nop; nop;");
    gpio_put(CS_PIN, 1);
}

void dac_write(uint channel, uint16_t value) {
    value &= 0x3FF; // 10-bit
    uint16_t cmd = (channel << 15) | (1<<14) | (1<<13) | (1<<12) | (value << 2);
    uint8_t buf[2] = { cmd >> 8, cmd & 0xFF };

    cs_select();
    spi_write_blocking(spi_default, buf, 2);
    cs_deselect();
}

int main() {
    stdio_init_all();

    spi_init(spi_default, 12000);
    gpio_set_function(16, GPIO_FUNC_SPI);
    gpio_set_function(18, GPIO_FUNC_SPI);
    gpio_set_function(19, GPIO_FUNC_SPI);

    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);

    const float dt = 0.0002f; // 200 us → 5 kHz update
    while (1) {
        // 2 Hz sine on channel A
        for (float t=0; t<1.0f; t+=dt) {
            float s = (sinf(2*M_PI*2*t)+1)*0.5f;
            dac_write(0, (uint16_t)(s*1023));
            sleep_us(200);
        }

        // 1 Hz triangle on channel B
        for (int i=0; i<1024; i++) {
            dac_write(1, i);
            sleep_us(200);
        }
        for (int i=1023; i>=0; i--) {
            dac_write(1, i);
            sleep_us(200);
        }
    }
}
