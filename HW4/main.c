#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

int main() {

    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    i2c_init(i2c_default, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    ssd1306_setup();
    ssd1306_clear();

    absolute_time_t t0;
    absolute_time_t t1;
    uint32_t dt;
    float fps;

    char msg1[40];
    char msg2[40];

    while (1) {

        gpio_put(25, 1);
        sleep_ms(50);
        gpio_put(25, 0);

        t0 = get_absolute_time();

        uint16_t raw = adc_read();
        float v = raw * 3.3f / 4095.0f;

        ssd1306_clear();

        sprintf(msg1, "Voltage %.2f V", v);
        drawString(0, 0, msg1);

        t1 = get_absolute_time();
        dt = absolute_time_diff_us(t0, t1);
        fps = 1000000.0f / dt;

        sprintf(msg2, "FPS %.2f", fps);
        drawString(0, 16, msg2);

        ssd1306_update();
    }

    return 0;
}
