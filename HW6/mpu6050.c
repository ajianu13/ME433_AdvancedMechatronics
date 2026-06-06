#include "mpu6050.h"
#define PICO_DEFAULT_LED_PIN 25


static void write_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(MPU_I2C, MPU_ADDR, buf, 2, false);
}

static uint8_t read_reg(uint8_t reg) {
    uint8_t v;
    i2c_write_blocking(MPU_I2C, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(MPU_I2C, MPU_ADDR, &v, 1, false);
    return v;
}

void mpu6050_init() {
    sleep_ms(100);

    // Try WHO_AM_I but DO NOT lock up if it fails
    uint8_t who = 0x00;
    uint8_t reg = WHO_AM_I;

    // Attempt read, but ignore failure
    i2c_write_blocking(MPU_I2C, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(MPU_I2C, MPU_ADDR, &who, 1, false);

    // If IMU not found, just continue anyway
    // (mouse will still work, IMU mode will just give dx=dy=0)
    // No infinite loop!

    // Wake up device (ignore errors)
    write_reg(PWR_MGMT_1, 0x00);
    write_reg(ACCEL_CONFIG, 0x00);
    write_reg(GYRO_CONFIG, 0x18);
}


bool mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az,
                      int16_t *gx, int16_t *gy, int16_t *gz,
                      int16_t *temp) {

    uint8_t start = ACCEL_XOUT_H;
    uint8_t data[14];

    i2c_write_blocking(MPU_I2C, MPU_ADDR, &start, 1, true);
    i2c_read_blocking(MPU_I2C, MPU_ADDR, data, 14, false);

    *ax   = (data[0] << 8) | data[1];
    *ay   = (data[2] << 8) | data[3];
    *az   = (data[4] << 8) | data[5];
    *temp = (data[6] << 8) | data[7];
    *gx   = (data[8] << 8) | data[9];
    *gy   = (data[10] << 8) | data[11];
    *gz   = (data[12] << 8) | data[13];

    return true;
}

bool mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    int16_t gx, gy, gz, temp;
    return mpu6050_read_raw(ax, ay, az, &gx, &gy, &gz, &temp);
}

int accel_to_speed(int16_t a) {
    int16_t abs_a = (a < 0) ? -a : a;

    if (abs_a < 1000) {
        return 0;
    } else if (abs_a < 4000) {
        return 1;
    } else if (abs_a < 10000) {
        return 3;
    } else {
        return 5;
    }
}
