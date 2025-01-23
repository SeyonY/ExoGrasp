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

void Process_ADC_Data(ADC_HandleTypeDef* hadc1, uint16_t* adc_buffer, uint16_t* sensor_averages)
{
    uint16_t current_pos;
    uint32_t sensor_sums[NUM_ADC_CHANNELS] = {0}; // To store sums for each channel
    uint16_t total_samples_per_channel = SAMPLES_PER_CHANNEL;

    // Get the current DMA index (how many transfers are left)
    current_pos = TOTAL_SAMPLES - __HAL_DMA_GET_COUNTER(hadc1->DMA_Handle);

    // Traverse the last 50ms worth of data in the circular buffer
    for (uint32_t i = 0; i < total_samples_per_channel; i++)
    {
        for (uint32_t channel = 0; channel < NUM_ADC_CHANNELS; channel++)
        {
            // Calculate the buffer index for each sample and channel
            uint32_t index = (current_pos + i * NUM_ADC_CHANNELS + channel) % TOTAL_SAMPLES;
            sensor_sums[channel] += adc_buffer[index];
        }
    }

    // Calculate averages for each channel
    for (uint32_t channel = 0; channel < NUM_ADC_CHANNELS; channel++)
    {
        sensor_averages[channel] = sensor_sums[channel] / total_samples_per_channel;
    }
}

