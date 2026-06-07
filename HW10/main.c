#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

#define BUTTON_PIN 14

int main() {
    stdio_init_all();

    // I2C + MPU6050
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    mpu6050_init();

    // Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    sleep_ms(2000);  // wait for USB serial

    while (1) {
        int16_t ax_raw, ay_raw, az_raw;
        mpu6050_read_accel(&ax_raw, &ay_raw, &az_raw);

        float ax = ax_raw / 16384.0f;
        float ay = ay_raw / 16384.0f;
        float az = az_raw / 16384.0f;

        int btn = !gpio_get(BUTTON_PIN);  // pressed = 1

        printf("BTN,%d,AX,%f,AY,%f,AZ,%f\n", btn, ax, ay, az);
        sleep_ms(20);
    }
}
