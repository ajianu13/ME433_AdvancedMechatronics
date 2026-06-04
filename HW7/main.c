// MCP4912 Pin → Pico Pin

// 1  VDD    3V3
// 12 VSS    GND

// 3  CS     GP17
// 4  SCK    GP18
// 5  SDI    GP19

// 8  LDAC   GND
// 9  SHDN   3V3

// 13 VREFA  3V3
// 11 VREFB  3V3

// 14 VOUTA  GP26 (ADC0)
// 10 VOUTB  GP27 (ADC1)

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include <math.h>
#include <stdio.h>


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

    value &= 0x3FF;

    uint16_t cmd = 0;
    cmd |= (channel & 1) << 15;
    cmd |= 1 << 14;
    cmd |= 1 << 13;
    cmd |= 1 << 12;
    cmd |= value << 2;

    uint8_t buf[2];
    buf[0] = cmd >> 8;
    buf[1] = cmd & 0xFF;

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

    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    const float dt = 0.0002f;

    while (1) {

        for (float t = 0; t < 1.0f; t += dt) {

            float s = sinf(2 * M_PI * 2 * t);
            float s_norm = (s + 1.0f) * 0.5f;
            uint16_t s_val = (uint16_t)(s_norm * 1023.0f);
            dac_write(0, s_val);

            float p = fmodf(t, 1.0f);
            float tri;
            if (p < 0.5f) {
                tri = p * 2.0f;
            } else {
                tri = (1.0f - p) * 2.0f;
            }
            uint16_t tri_val = (uint16_t)(tri * 1023.0f);
            dac_write(1, tri_val);

            adc_select_input(0);
            uint16_t a = adc_read();
            adc_select_input(1);
            uint16_t b = adc_read();

            printf("%u,%u\n", a, b);

            sleep_us(200);
        }
    }

    return 0;
}
