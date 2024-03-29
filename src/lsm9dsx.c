#ifdef SERIES0
#include "lsm9ds0.h"
#elif SERIES1
#include "lsm9ds1.h"
#endif

#include <stdint.h>
#include "i2c.h"

void setI2CMag() {
    setI2CDev(MAG_ADDR);
}

void setI2CAcc() {
    setI2CDev(ACC_ADDR);
}

void setI2CGyr() {
    setI2CDev(GYR_ADDR);
}

// TODO: Check Series0 inits

void initAcc(int range) {
    setI2CAcc();
#ifdef SERIES0
    writeByte(CTRL_REG1_A, 0xA7);
    writeByte(CTRL_REG2_A, range);
#elif SERIES1
    writeByte(CTRL_REG5_A, 0x38);   // Enable X, Y, Z
    writeByte(CTRL_REG6_A, 0xC0);   // 1 KHz out data rate, BW set by ODR, 408Hz anti-aliasing
    uint8_t reg = readByte(CTRL_REG6_A);
    reg &= ~(0b00011000);
    reg |= range;
    writeByte(CTRL_REG6_A, reg);
#endif
}

void initGyr(int scale) {
    setI2CGyr();
#ifdef SERIES0
    writeByte(CTRL_REG1_G, 0x0F);
    writeByte(CTRL_REG4_G, scale);
#elif SERIES1
    writeByte(CTRL_REG1_G, 0xC0);   // Enable X, Y, Z
    uint8_t reg = readByte(CTRL_REG1_G);
    reg &= ~(0b00011000);
    reg |= scale;
    writeByte(CTRL_REG1_G, reg);
#endif
}

void initMag(int gain) {
    setI2CMag();
#ifdef SERIES0
    writeByte(CTRL_REG5_M, 0xF0);
    writeByte(CTRL_REG6_M, gain);
    writeByte(CTRL_REG7_M, 0x00);
#elif SERIES1
    writeByte(CTRL_REG3_M, 0x00);   // Continuous mode
    uint8_t reg = readByte(CTRL_REG2_M);
    reg &= ~(0b01100000);
    reg |= gain;
    writeByte(CTRL_REG2_M, reg);

#endif
}

void readMag(float res[3], float conversion) {
    setI2CMag();
	uint8_t xLo = readByte(OUT_X_L_M);
	uint8_t yLo = readByte(OUT_Y_L_M);
	uint8_t zLo = readByte(OUT_Z_L_M);
    int16_t xHi = ((int16_t)readByte(OUT_X_H_M)) << 8;
    int16_t yHi = ((int16_t)readByte(OUT_Y_H_M)) << 8;
    int16_t zHi = ((int16_t)readByte(OUT_Z_H_M)) << 8;
    res[0] = (float)((xHi | xLo) * conversion / 1000);
    res[1] = (float)((yHi | yLo) * conversion / 1000);
    res[2] = (float)((zHi | zLo) * conversion / 1000); 
};

void readAcc(float res[3], float conversion) {
    setI2CAcc();
	uint8_t xLo = readByte(OUT_X_L_A);
	uint8_t yLo = readByte(OUT_Y_L_A);
	uint8_t zLo = readByte(OUT_Z_L_A);
    int16_t xHi = ((int16_t)readByte(OUT_X_H_A)) << 8;
    int16_t yHi = ((int16_t)readByte(OUT_Y_H_A)) << 8;
    int16_t zHi = ((int16_t)readByte(OUT_Z_H_A)) << 8;
    res[0] = (float)((xHi | xLo) * conversion / 1000) * GRAVITY;
    res[1] = (float)((yHi | yLo) * conversion / 1000) * GRAVITY;
    res[2] = (float)((zHi | zLo) * conversion / 1000) * GRAVITY; 
};

void readGyr(float res[3], float conversion) {
    setI2CGyr();
	uint8_t xLo = readByte(OUT_X_L_G);
	uint8_t yLo = readByte(OUT_Y_L_G);
	uint8_t zLo = readByte(OUT_Z_L_G);
    int16_t xHi = ((int16_t)readByte(OUT_X_H_G)) << 8;
    int16_t yHi = ((int16_t)readByte(OUT_Y_H_G)) << 8;
    int16_t zHi = ((int16_t)readByte(OUT_Z_H_G)) << 8;
    res[0] = (float)((xHi | xLo) * conversion);
    res[1] = (float)((yHi | yLo) * conversion);
    res[2] = (float)((zHi | zLo) * conversion); 
};

// TODO: Implement reading temperature sensor

void init_imu() {
    openI2CBus("/dev/i2c-1");
    // TODO: Softreset each sensor
    // TODO: Check WHO_AM_I
    initAcc(ACCELRANGE_16G);
    initGyr(GYROSCALE_500DPS);
}

void get_imu_reading(float acc[3], float gyr[3]) {
    readAcc(acc, ACCEL_MG_LSB_16G);
    readGyr(gyr, GYRO_DPS_DIGIT_500DPS);
}
