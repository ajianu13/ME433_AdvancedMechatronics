#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

static const uint8_t addr = 0x3C;
static uint8_t buf[513];

static void write_cmd(uint8_t c) {
    uint8_t m[2];
    m[0] = 0x00;
    m[1] = c;
    i2c_write_blocking(i2c_default, addr, m, 2, false);
}

void ssd1306_setup(void) {
    buf[0] = 0x40;
    sleep_ms(30);

    write_cmd(SSD1306_DISPLAYOFF);
    write_cmd(SSD1306_SETDISPLAYCLOCKDIV);
    write_cmd(0x80);
    write_cmd(SSD1306_SETMULTIPLEX);
    write_cmd(0x1F);
    write_cmd(SSD1306_SETDISPLAYOFFSET);
    write_cmd(0x00);
    write_cmd(SSD1306_SETSTARTLINE);
    write_cmd(SSD1306_CHARGEPUMP);
    write_cmd(0x14);
    write_cmd(SSD1306_MEMORYMODE);
    write_cmd(0x00);
    write_cmd(SSD1306_SEGREMAP | 0x01);
    write_cmd(SSD1306_COMSCANDEC);
    write_cmd(SSD1306_SETCOMPINS);
    write_cmd(0x02);
    write_cmd(SSD1306_SETCONTRAST);
    write_cmd(0x8F);
    write_cmd(SSD1306_SETPRECHARGE);
    write_cmd(0xF1);
    write_cmd(SSD1306_SETVCOMDETECT);
    write_cmd(0x40);
    write_cmd(SSD1306_DISPLAYON);

    ssd1306_clear();
    ssd1306_update();
}

void ssd1306_update(void) {
    write_cmd(SSD1306_PAGEADDR);
    write_cmd(0x00);
    write_cmd(0x03);

    write_cmd(SSD1306_COLUMNADDR);
    write_cmd(0x00);
    write_cmd(0x7F);

    i2c_write_blocking(i2c_default, addr, buf, 513, false);
}

void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) {
    if (x > 127 || y > 31) return;

    uint16_t index = 1 + x + (y >> 3) * 128;
    uint8_t bit = 1 << (y & 7);

    if (color) {
        buf[index] |= bit;
    } else {
        buf[index] &= ~bit;
    }
}

void ssd1306_clear(void) {
    memset(buf, 0, 513);
    buf[0] = 0x40;
}
