
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
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"
#include "ssd1306.h"

#define OLED_I2C i2c0
#define OLED_SDA 8
#define OLED_SCL 9

#define IMU_SDA 14
#define IMU_SCL 15

void drawLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        ssd1306_drawPixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


int main() {
    stdio_init_all();

    // I2C init
    i2c_init(OLED_I2C, 400000);
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    // OLED init
    ssd1306_setup();

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
        int cy = 16;
        float scale = 25.0f;

        int x2 = cx + (int)(ax_g * scale);
        int y2 = cy + (int)(ay_g * scale);

        // ssd1306_clear(&oled);
        // // Different Library, can't combine
        // // ssd1306_line(&oled, cx, cy, x2, y2, 1);
        // ssd1306_show(&oled);

        ssd1306_clear();
        drawLine(cx, cy, x2, y2);
        ssd1306_update();


        sleep_ms(10); // ~100 Hz
    }
}


