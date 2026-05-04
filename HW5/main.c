#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"
#include "ssd1306.h"   // your OLED library

int main() {
    stdio_init_all();

    // I2C init
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    // OLED init
    ssd1306_t oled;
    ssd1306_init(&oled, 128, 64, false, 0x3C, i2c0);

    // MPU init
    mpu6050_init();

    while (1) {
        int16_t ax, ay, az, gx, gy, gz, temp;
        mpu6050_read_raw(&ax, &ay, &az, &gx, &gy, &gz, &temp);

        float ax_g = ax * 0.000061;
        float ay_g = ay * 0.000061;

        int cx = 64;
        int cy = 32;
        float scale = 25.0;

        int x2 = cx + (int)(ax_g * scale);
        int y2 = cy + (int)(ay_g * scale);

        ssd1306_clear(&oled);
        ssd1306_line(&oled, cx, cy, x2, y2, 1);
        ssd1306_show(&oled);

        sleep_ms(10); // ~100 Hz
    }
}
