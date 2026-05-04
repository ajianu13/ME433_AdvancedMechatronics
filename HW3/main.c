/*
WIRING NOTES FOR HW3

I used GP4 on the Pico 2 W as the SDA line and connected it to the SDA pin on the MCP23008.  
I used GP5 on the Pico 2 W as the SCL line and connected it to the SCL pin on the MCP23008.  

Both SDA and SCL are pulled up to 3.3 volts with external resistors, since the MCP23008 needs proper pull‑ups for I2C to work.

The MCP23008 is powered from the Pico’s 3.3 volt pin and shares a common ground with the Pico.  
The address pins A0, A1, and A2 are all tied to ground so the chip uses address 0x20.  
The reset pin on the MCP23008 is tied to 3.3 volts so the chip stays active.

The button is connected to GPA0 on the MCP23008 and the other side of the button goes to ground.  
Pressing the button pulls GPA0 low.

The LED is connected to GPA7 on the MCP23008.  
The LED has a resistor in series and the other side of the LED goes to ground.  
When GPA7 is driven high, the LED turns on.

The onboard LED on the Pico (GP16) is used as a heartbeat to show the program is running.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define MCP_ADDR 0x20
#define IODIR   0x00
#define GPIO    0x09
#define OLAT    0x0A
#define HEARTBEAT_LED 16


void mcp_write(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};

    // Send Data
    int result = i2c_write_blocking(i2c_default, MCP_ADDR, buf, 2, false);

    printf("WRITE reg 0x%02X <- 0x%02X   (result=%d)\n", reg, value, result);
}

uint8_t mcp_read(uint8_t reg) {
    uint8_t value = 0xFF;

    // Read Data
    int w = i2c_write_blocking(i2c_default, MCP_ADDR, &reg, 1, true);
    int r = i2c_read_blocking(i2c_default, MCP_ADDR, &value, 1, false);

    printf("READ reg 0x%02X -> 0x%02X   (w=%d, r=%d)\n", reg, value, w, r);

    return value;
}

int main() {
    stdio_init_all();
    sleep_ms(2000);  // give USB time to connect

    printf("Starting HW3 program...\n");

    // Init I2C
    i2c_init(i2c_default, 100000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    printf("I2C initialized on GP4/GP5\n");

    // Heartbeat LED
    gpio_init(HEARTBEAT_LED);
    gpio_set_dir(HEARTBEAT_LED, GPIO_OUT);

    // Configure MCP23008
    printf("Configuring MCP23008...\n");
    mcp_write(IODIR, 0x7F);   // GP0 input, GP7 output
    mcp_write(OLAT, 0x00);    // LED off

    printf("MCP23008 setup complete.\n");

    while (true) {
        // Heartbeat
        gpio_put(HEARTBEAT_LED, 1);
        sleep_ms(100);
        gpio_put(HEARTBEAT_LED, 0);

        // Read GPIO register
        uint8_t gpio_state = mcp_read(GPIO);
        uint8_t button = gpio_state & 0x01;

        printf("Button raw state: %d\n", button);

        if (button == 0) {
            printf("Button PRESSED -> LED ON\n");
            mcp_write(OLAT, 0b10000000);
        } else {
            printf("Button released -> LED OFF\n");
            mcp_write(OLAT, 0b00000000);
        }

        sleep_ms(200);
    }
}
