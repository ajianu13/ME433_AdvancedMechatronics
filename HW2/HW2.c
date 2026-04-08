#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define SERVO_PIN 16

// Function to set servo pulse width in microseconds
void set_servo_micros(uint gpio, uint32_t micros) {
    // Pico 2 clock is typically 150MHz or 125MHz. 
    // We set the divider so 1 count = 1 microsecond.
    pwm_set_chan_level(pwm_gpio_to_slice_num(gpio), pwm_gpio_to_channel(gpio), micros);
}

int main() {
    stdio_init_all();

    // Set up PWM for the Servo Pin
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);

    // Get the default clock speed and set divider to get 1MHz (1 tick per us)
    // 150MHz / 150.0 = 1MHz
    // pwm_set_clkdiv(slice_num, 150.0f); 
    // The pico runs at 150000000 cycles per second
    // We want to consider every 150 cycles as a tick
    // Not getting full range of motion, let's just read how many cycles it does

    // Calculate divider to achieve a 1MHz PWM tick rate.
    // Formula: clk_sys / (div * wrap) = frequency
    // By setting div to (clock_hz / 1,000,000), each 'tick' in the wrap/level represents exactly 1 microsecond.
    float clock_hz = (float)clock_get_hz(clk_sys);
    pwm_set_clkdiv(slice_num, clock_hz / 1000000.0f);
    
    // Set period to 20,000us (50Hz)
    // Set the PWM wrap point to 20,000 ticks.
    // At 1MHz tick rate, 20,000 ticks = 20ms period, which equals 50Hz.
    // 50Hz is the standard operating frequency for most RC analog servos.
    pwm_set_wrap(slice_num, 20000);
    pwm_set_enabled(slice_num, true);

    // Standard Servo Pulse Widths:
    // 500us  = 2.5% duty cycle  -> 0 degrees
    // 1500us = 7.5% duty cycle  -> 90 degrees (neutral)
    // 2500us = 12.5% duty cycle -> 180 degrees

    while (true) {
        // Sweep from 0 to 180 degrees (500us to 2500us)
        for (int i = 500; i <= 2500; i += 10) {
            set_servo_micros(SERVO_PIN, i);     // Command the servo to move to a specific degree
            sleep_ms(10);                       // 10ms delay to allow movement to happen
        }
        // Sweep back
        for (int i = 2500; i >= 500; i -= 10) {
            set_servo_micros(SERVO_PIN, i);
            sleep_ms(10);
        }
    }
}
