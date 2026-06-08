#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

int main() {
    stdio_init_all();

    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);   // TX
    gpio_set_function(1, GPIO_FUNC_UART);   // RX

    while (true) {

        if (uart_is_readable(uart0)) {
            char r = uart_getc(uart0);
            putchar_raw(r);
        }

        int k = getchar_timeout_us(0);
        if (k >= 0) {
            uart_putc_raw(uart0, (char)k);
        }
    }
}
