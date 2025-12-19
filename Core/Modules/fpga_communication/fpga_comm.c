/*
 * fpga_comm.c
 *
 *  Created on: Dec 14, 2025
 *      Author: Nam Truong
 */

#include "fpga_communication/fpga_comm.h"

/* * SƠ ĐỒ NỐI DÂY (CHIPKIT HEADER)
 * PB13 (SCK)  -> H15 (CK_SCK)
 * PB15 (MOSI) -> T12 (CK_MOSI)
 * PB12 (CS)   -> F16 (CK_SS)
 * GND         -> GND
 */

extern SPI_HandleTypeDef hspi2;

void FPGA_Send_Image(uint8_t *rgb_buffer, uint8_t width, uint8_t height)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi2, rgb_buffer, 160*120, 1000);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

