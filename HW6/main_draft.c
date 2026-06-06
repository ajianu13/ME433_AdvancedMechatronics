#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "mpu6050.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <math.h>

#define BUTTON_PIN 14
#define LED_PIN 15

int main() {
    //stdio_init_all();

    // I2C + MPU6050
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    //mpu6050_init();

    // Button + LED
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    tusb_init();

    bool mode = false;
    bool last_button = true;
    float theta = 0;

    while (1) {
        tud_task();  // USB

        // Button toggle
        bool b = gpio_get(BUTTON_PIN);
        if (!b && last_button) {
            mode = !mode;
            gpio_put(LED_PIN, mode);
        }
        last_button = b;

        int dx = 0, dy = 0;

        if (!mode) {
            // // IMU mode
            // int16_t ax, ay, az;
            // mpu6050_read_accel(&ax, &ay, &az);
            // dx = accel_to_speed(ax);
            // dy = accel_to_speed(-ay);
            dx = 3;
            dy = 0;
        } else {
            // Circle mode
            dx = (int)(3 * cos(theta));
            dy = (int)(3 * sin(theta));
            theta += 0.05;
        }

        tud_hid_mouse_report(REPORT_ID_MOUSE, 0, dx, dy, 0, 0);
        // tud_hid_mouse_report(REPORT_ID_MOUSE, buttons, dx, dy, 0, 0);
        sleep_ms(10);
    }
}


// // Testing if I can control cursor without the IMU
// #include "pico/stdlib.h"
// #include "tusb.h"
// #include "usb_descriptors.h"

// #define LED_PIN 15

// int main() {
//     stdio_init_all();

//     gpio_init(LED_PIN);
//     gpio_set_dir(LED_PIN, GPIO_OUT);

//     tusb_init();

//     int8_t dx = 2;
//     int8_t dy = 0;
//     uint32_t last_ms = 0;

//     while (1) {
//         tud_task();

//         // LED on when mounted
//         gpio_put(LED_PIN, tud_mounted());

//         // Every 20 ms, send a tiny mouse move in a line
//         uint32_t now = to_ms_since_boot(get_absolute_time());
//         if (tud_hid_ready() && (now - last_ms) > 20) {
//             last_ms = now;
//             tud_hid_mouse_report(REPORT_ID_MOUSE, 0, dx, dy, 0, 0);
//         }
//     }
// }
