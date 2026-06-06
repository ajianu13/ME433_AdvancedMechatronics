#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <math.h>

#define LED_PIN 15
#define BUTTON_PIN 14

int main() {
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    mpu6050_init();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    tusb_init();

    bool mode = false;
    bool last = true;
    float theta = 0;

    while (1) {
        tud_task();

        bool b = gpio_get(BUTTON_PIN);
        if (!b && last) {
            mode = !mode;
            gpio_put(LED_PIN, mode);
        }
        last = b;

        int dx = 0;
        int dy = 0;

        if (!mode) {
            int16_t ax, ay, az;
            bool ok = mpu6050_read_accel(&ax, &ay, &az);

            if (ok) {
                int sx = accel_to_speed(ax);
                int sy = accel_to_speed(ay);

                dx = (ax < 0) ? -sx : sx;
                dy = (ay < 0) ? -sy : sy;
            }
        } else {
            dx = (int)(3 * cos(theta));
            dy = (int)(3 * sin(theta));
            theta += 0.05f;
        }

        if (tud_hid_ready()) {
            tud_hid_mouse_report(REPORT_ID_MOUSE, 0, dx, dy, 0, 0);
        }

        sleep_ms(10);
    }
}
