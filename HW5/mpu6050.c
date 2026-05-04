#include "mpu6050.h"

static void write_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(i2c0, MPU_ADDR, buf, 2, false);
}

static uint8_t read_reg(uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(i2c0, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU_ADDR, &val, 1, false);
    return val;
}

void mpu6050_init() {
    sleep_ms(100);

    uint8_t who = read_reg(WHO_AM_I);
    if (who != 0x68 && who != 0x98) {
        while (1) {
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(200);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(200);
        }
    }

    write_reg(PWR_MGMT_1, 0x00);   // wake up
    write_reg(ACCEL_CONFIG, 0x00); // ±2g
    write_reg(GYRO_CONFIG, 0x18);  // ±2000 dps
}

bool mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az,
                      int16_t *gx, int16_t *gy, int16_t *gz,
                      int16_t *temp) {

    uint8_t reg = ACCEL_XOUT_H;
    uint8_t data[14];

    i2c_write_blocking(i2c0, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU_ADDR, data, 14, false);

    *ax = (data[0] << 8) | data[1];
    *ay = (data[2] << 8) | data[3];
    *az = (data[4] << 8) | data[5];
    *temp = (data[6] << 8) | data[7];
    *gx = (data[8] << 8) | data[9];
    *gy = (data[10] << 8) | data[11];
    *gz = (data[12] << 8) | data[13];

    return true;
}
