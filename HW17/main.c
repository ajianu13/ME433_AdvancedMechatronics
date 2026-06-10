#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C1_SDA 14
#define I2C1_SCL 15
#define AS5600_ADDR 0x36

void as5600_init(void) {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA);
    gpio_pull_up(I2C1_SCL);
}

uint16_t as5600_read_raw(void) {
    uint8_t reg = 0x0C;
    uint8_t buf[2];
    i2c_write_blocking(i2c1, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c1, AS5600_ADDR, buf, 2, false);
    return ((buf[0] & 0x0F) << 8) | buf[1];
}

float as5600_read_deg(void) {
    return as5600_read_raw() * (360.0f / 4096.0f);
}

int main() {
    stdio_init_all();
    as5600_init();
    sleep_ms(1000);

    while (1) {
        float angle = as5600_read_deg();
        printf("%0.2f\n", angle);
        sleep_ms(10);
    }
}
