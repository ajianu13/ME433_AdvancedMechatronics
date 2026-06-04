
// OLED Pico
// SDA	GP8
// SCL	GP9
// VCC	3.3V
// GND	GND


// MPU6050 Pico
// VCC	3.3V
// GND	GND
// SDA	GP14
// SCL	GP15

// The OLED uses I2C0
// The IMU uses I2C1
// Both have built in pull-ups

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"
#include "ssd1306.h"

#define OLED_I2C i2c0
#define OLED_SDA 8
#define OLED_SCL 9

#define IMU_SDA 14
#define IMU_SCL 15

int main() {
    stdio_init_all();

    // I2C init
    i2c_init(OLED_I2C, 400000);
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    // OLED init
    ssd1306_t oled;
    ssd1306_init(&oled, 128, 64, false, 0x3C, OLED_I2C);

    // IMU setup
    i2c_init(MPU_I2C, 400000);
    gpio_set_function(IMU_SDA, GPIO_FUNC_I2C);
    gpio_set_function(IMU_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_SDA);
    gpio_pull_up(IMU_SCL);

    // MPU init
    mpu6050_init();

    while (1) {
        int16_t ax, ay, az, gx, gy, gz, temp;
        mpu6050_read_raw(&ax, &ay, &az, &gx, &gy, &gz, &temp);

        float ax_g = ax * 0.000061f;
        float ay_g = ay * 0.000061f;

        int cx = 64;
        int cy = 32;
        float scale = 25.0f;

        int x2 = cx + (int)(ax_g * scale);
        int y2 = cy + (int)(ay_g * scale);

        ssd1306_clear(&oled);
        ssd1306_line(&oled, cx, cy, x2, y2, 1);
        ssd1306_show(&oled);

        sleep_ms(10); // ~100 Hz
    }
}
