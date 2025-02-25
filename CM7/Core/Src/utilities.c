#include "utilities.h"

HAL_StatusTypeDef readPressureSensor(I2C_HandleTypeDef hi2c, float* pressure) {
	HAL_StatusTypeDef status;
	uint8_t buf[3] = {0};
	uint8_t cmd = CONVERSION_CMD;
	int32_t rawPressure;

	// Send command to command register to prepare for data read
	status = HAL_I2C_Mem_Write(&hi2c, XGZP6857D_I2C_ADDR, REG_CMD, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
	if (status != HAL_OK) {
		return status;
	}

	osDelay(CONVERSION_TIME_MS);

	// Read in data from pressure registers
	status = HAL_I2C_Mem_Read(&hi2c, XGZP6857D_I2C_ADDR, REG_PRESSURE_MSB, I2C_MEMADD_SIZE_8BIT, buf, 3, 100);
	if (status != HAL_OK) {
		return status;
	}

	// Combine pressure data from different bytes
	rawPressure = (buf[0] << 16) | (buf[1] << 8) | buf[2];

	// Check if pressure value is negative
	if (rawPressure & 0x800000) {
		rawPressure -= (1 << 24);
	}

	*pressure = (float)rawPressure/K_FACTOR;
	return status;
}

void Process_ADC_Data(uint16_t* adc_buffer, uint16_t* sensor_averages)
{
    uint32_t sensor_sums[NUM_ADC_CHANNELS] = {0}; // To store sums for each channel

    // Traverse the last 50ms worth of data in the circular buffer
    for (uint16_t i = 0; i < TOTAL_SAMPLES; i++)
    {
		// Calculate the buffer index for each sample and channel
    	int channel = i % 4;
    	uint16_t adc_value = adc_buffer[i];
		sensor_sums[channel] += adc_value;
    }

    // Calculate averages for each channel
    for (uint16_t channel = 0; channel < NUM_ADC_CHANNELS; channel++)
    {
        sensor_averages[channel] = sensor_sums[channel] / SAMPLES_PER_CHANNEL;
    }
}

uint32_t get_dma_position(DMA_HandleTypeDef hdma_adc1) {
    return (TOTAL_SAMPLES - __HAL_DMA_GET_COUNTER(&hdma_adc1)) % TOTAL_SAMPLES;
}
