/*
 * OV7670.h
 *
 *  Created on: Dec 4, 2025
 *      Author: Nam Truong
 */

#ifndef MODULES_CAMERA_OV7670_H_
#define MODULES_CAMERA_OV7670_H_


void ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c);
void ov7670_config();
void ov7670_startCap(uint32_t capMode, uint32_t destAddress);
void ov7670_stopCap();


#endif /* MODULES_CAMERA_OV7670_H_ */
