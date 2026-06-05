#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SPI_PORT spi0
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_MISO 16
#define PIN_CS_DAC 17
#define PIN_CS_RAM 20

#define RAM_CMD_WRITE 0x02
#define RAM_CMD_READ  0x03
#define RAM_CMD_WRSR  0x01
#define RAM_MODE_SEQ  0x40

static inline void cs_select(uint pin) {
    gpio_put(pin, 0);
}

static inline void cs_deselect(uint pin) {
    gpio_put(pin, 1);
}

void spi_ram_init() {
    spi_init(SPI_PORT, 4 * 1000 * 1000);

    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS_DAC);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    cs_deselect(PIN_CS_DAC);

    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    cs_deselect(PIN_CS_RAM);

    uint8_t cmd[2] = {RAM_CMD_WRSR, RAM_MODE_SEQ};
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, cmd, 2);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_write_byte(uint16_t addr, uint8_t data) {
    uint8_t buf[4];
    buf[0] = RAM_CMD_WRITE;
    buf[1] = (addr >> 8) & 0xFF;
    buf[2] = addr & 0xFF;
    buf[3] = data;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, buf, 4);
    cs_deselect(PIN_CS_RAM);
}

uint8_t spi_ram_read_byte(uint16_t addr) {
    uint8_t header[3];
    header[0] = RAM_CMD_READ;
    header[1] = (addr >> 8) & 0xFF;
    header[2] = addr & 0xFF;

    uint8_t result;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_read_blocking(SPI_PORT, 0x00, &result, 1);
    cs_deselect(PIN_CS_RAM);

    return result;
}

// Multi bit read
void spi_ram_read_bytes(uint16_t addr, uint8_t *buf, int len) {
    uint8_t header[3];
    header[0] = RAM_CMD_READ;
    header[1] = (addr >> 8) & 0xFF;
    header[2] = addr & 0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_read_blocking(SPI_PORT, 0x00, buf, len);
    cs_deselect(PIN_CS_RAM);
}


void generate_sine_to_ram() {
    for (int n = 0; n < 1000; n++) {
        float t = (float)n / 1000.0f;
        float v = (sinf(2.0f * M_PI * t) + 1.0f) * 1.65f;

        uint16_t code = (uint16_t)((v / 3.3f) * 1023.0f);

        uint16_t word = 0;
        word |= 0 << 15;
        word |= 1 << 14;
        word |= 1 << 13;
        word |= 1 << 12;
        word |= (code << 2);

        uint8_t hi = (word >> 8) & 0xFF;
        uint8_t lo = word & 0xFF;

        uint16_t addr = n * 2;
        spi_ram_write_byte(addr, hi);
        spi_ram_write_byte(addr + 1, lo);
    }
}

void dac_write_word(uint16_t word) {
    uint8_t buf[2];
    buf[0] = (word >> 8) & 0xFF;
    buf[1] = word & 0xFF;

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect(PIN_CS_DAC);
}

void playback_loop() {
    while (1) {
        for (int n = 0; n < 1000; n++) {
            uint16_t addr = n * 2;
            uint8_t hi = spi_ram_read_byte(addr);
            uint8_t lo = spi_ram_read_byte(addr + 1);

            // DEBUGG
            uint8_t check[2];
            spi_ram_read_bytes(addr, check, 2);
            //printf("n=%d WROTE %02X %02X READ %02X %02X\n", n, hi, lo, check[0], check[1]);
            // DEBUGG
            uint16_t word = ((uint16_t)hi << 8) | lo;

            printf("%u\n", word);

            dac_write_word(word);
            sleep_ms(1);
        }
    }
}

int main() {
    stdio_init_all();
    sleep_ms(500);

    spi_ram_init();
    generate_sine_to_ram();
    playback_loop();

    return 0;
}
