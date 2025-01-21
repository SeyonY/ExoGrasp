#include "main.h"
#include "cmsis_os.h"

#define XGZP6857D_I2C_ADDR      (0x6D<<1)
#define REG_PRESSURE_MSB         0x06
#define REG_PRESSURE_CSB         0x07
#define REG_PRESSURE_LSB         0x08
#define REG_CMD                  0x30

#define CONVERSION_CMD           0x0A
#define CONVERSION_TIME_MS       20
#define K_FACTOR                 16

HAL_StatusTypeDef readPressureSensor(I2C_HandleTypeDef hi2c, float* pressure);
