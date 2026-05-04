#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// -----------------------------
// SPI PIN DEFINITIONS
// -----------------------------
#define SPI_PORT spi0
#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_MISO 4
#define PIN_CS   5

// -----------------------------
// 23K256 RAM COMMANDS
// -----------------------------
#define RAM_CMD_WRITE 0x02
#define RAM_CMD_READ  0x03
#define RAM_CMD_WRSR  0x01
#define RAM_MODE_SEQ  0x40   // sequential mode

// -----------------------------
// CS CONTROL
// -----------------------------
static inline void cs_select() {
    gpio_put(PIN_CS, 0);
}

static inline void cs_deselect() {
    gpio_put(PIN_CS, 1);
}

// -----------------------------
// INITIALIZE SPI + RAM CHIP
// -----------------------------
void spi_ram_init() {
    spi_init(SPI_PORT, 4 * 1000 * 1000);  // 4 MHz

    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    cs_deselect();

    // Set RAM to sequential mode
    cs_select();
    uint8_t cmd[2] = {RAM_CMD_WRSR, RAM_MODE_SEQ};
    spi_write_blocking(SPI_PORT, cmd, 2);
    cs_deselect();
}

// -----------------------------
// WRITE ONE BYTE TO RAM
// -----------------------------
void spi_ram_write_byte(uint16_t addr, uint8_t data) {
    cs_select();
    uint8_t buf[4] = {
        RAM_CMD_WRITE,
        (addr >> 8) & 0xFF,
        addr & 0xFF,
        data
    };
    spi_write_blocking(SPI_PORT, buf, 4);
    cs_deselect();
}

// -----------------------------
// READ ONE BYTE FROM RAM
// -----------------------------
uint8_t spi_ram_read_byte(uint16_t addr) {
    cs_select();
    uint8_t header[3] = {
        RAM_CMD_READ,
        (addr >> 8) & 0xFF,
        addr & 0xFF
    };
    spi_write_blocking(SPI_PORT, header, 3);

    uint8_t result;
    spi_read_blocking(SPI_PORT, 0x00, &result, 1);
    cs_deselect();
    return result;
}

// -----------------------------
// DAC WRITE FUNCTION (YOU FILL THIS IN)
// -----------------------------
void dac_write(uint16_t value) {
    // TODO: Replace this with your DAC SPI code
    // Example for MCP4921:
    // uint8_t buf[2];
    // buf[0] = 0x30 | ((value >> 8) & 0x0F);
    // buf[1] = value & 0xFF;
    // spi_write_blocking(SPI_PORT, buf, 2);
}

// -----------------------------
// GENERATE 1000-SAMPLE SINE WAVE AND STORE IN RAM
// -----------------------------
void generate_sine_to_ram() {
    for (int n = 0; n < 1000; n++) {
        float t = (float)n / 1000.0f;
        float v = (sinf(2 * M_PI * t) + 1.0f) * 1.65f;  // 0–3.3V

        uint16_t dac = (uint16_t)((v / 3.3f) * 65535);

        uint16_t addr = n * 2;
        spi_ram_write_byte(addr, dac >> 8);
        spi_ram_write_byte(addr + 1, dac & 0xFF);
    }
}

// -----------------------------
// PLAYBACK LOOP (1 Hz SINE WAVE)
// -----------------------------
void playback_loop() {
    while (1) {
        for (int n = 0; n < 1000; n++) {
            uint16_t addr = n * 2;

            uint8_t hi = spi_ram_read_byte(addr);
            uint8_t lo = spi_ram_read_byte(addr + 1);
            uint16_t dac = (hi << 8) | lo;

            dac_write(dac);
            sleep_ms(1);  // 1000 samples/sec → 1 Hz
        }
    }
}

// -----------------------------
// MAIN
// -----------------------------
int main() {
    stdio_init_all();
    sleep_ms(500);

    spi_ram_init();
    generate_sine_to_ram();
    playback_loop();

    return 0;
}
