/*
 * OV7670.c
 *
 *  Created on: Dec 4, 2025
 *      Author: Nam Truong
 */


#include <stdio.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "camera/OV7670.h"
#include "camera/OV7670_config.h"
#include "camera/OV7670_reg.h"
#include "ILI9341/ili9341_config.h"
#include "ILI9341/ili9341.h"

/*** Internal Const Values, Macros ***/
#ifdef QVGA
#define OV7670_WIDTH  320
#define OV7670_HEIGHT 240
#endif

#ifdef QQVGA
#define OV7670_WIDTH  160
#define OV7670_HEIGHT 120
#endif


/*** Internal Static Variables ***/
static DCMI_HandleTypeDef *sp_hdcmi;
static DMA_HandleTypeDef  *sp_hdma_dcmi;
static I2C_HandleTypeDef  *sp_hi2c;
static uint32_t    s_destAddressForContiuousMode;
static uint32_t s_currentH;
static uint32_t s_currentV;

extern int frame_captured;
extern int cam_busy;
/*** Internal Function Declarations ***/
static void ov7670_write(uint8_t regAddr, uint8_t data);
static void ov7670_read(uint8_t regAddr, uint8_t *data);

/*** External Function Defines ***/
void ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c)
{
	sp_hdcmi     = p_hdcmi;
	sp_hdma_dcmi = p_hdma_dcmi;
	sp_hi2c      = p_hi2c;
	s_destAddressForContiuousMode = 0;

	HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);  // Hold reset
	HAL_Delay(10);
	HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);    // Release
	HAL_Delay(100);  // wait


	ov7670_write(0x12, 0x80);  // RESET
	HAL_Delay(30);

}

void ov7670_config()
{
	ov7670_stopCap();
	HAL_Delay(200);
	for(int i = 0; OV7670_reg[i][0] != REG_EXIT; i++)
	{
		ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
		HAL_Delay(1);
	}
}

void ov7670_startCap(uint32_t capMode, uint32_t destAddress)
{
	ov7670_stopCap();
	if (capMode == OV7670_CAP_CONTINUOUS) {
		/* note: continuous mode automatically invokes DCMI, but DMA needs to be invoked manually */
		s_destAddressForContiuousMode = (uint32_t)destAddress;
		HAL_DCMI_Start_DMA(sp_hdcmi,DCMI_MODE_CONTINUOUS,  destAddress, OV7670_WIDTH * OV7670_HEIGHT / 2);
	}
	else if (capMode == OV7670_CAP_SINGLE_FRAME) {
		s_destAddressForContiuousMode = 0;
		HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_SNAPSHOT, destAddress, OV7670_WIDTH * OV7670_HEIGHT / 2);
	}

}

void ov7670_stopCap()
{
	HAL_DCMI_Stop(sp_hdcmi);
	//  HAL_Delay(30);
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	frame_captured = 1;

	lcd_ILI_set_write_area(0, 0, LCD_ILI_WIDTH - 1, LCD_ILI_HEIGHT - 1); // set the address of lcd back to starting address

	if(s_destAddressForContiuousMode != 0) {
	HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode, OV7670_WIDTH * OV7670_HEIGHT/2);
	}
	s_currentV++;
	s_currentH = 0;
}
/*
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    static uint16_t row = 0;
    if(row >= 60 && row < 180) {
        uint16_t *dst = &fpga_buf[(row-60)*160];
        uint16_t *src = (uint16_t*)LCD_DATA_ADDR + 80;
        for(int i = 0; i < 160; i++)
            dst[i] = src[i];
    }
    row++;
    if(row == 240) row = 0;
}
*/
/* internal functions */
static void ov7670_write(uint8_t reg, uint8_t val)
{
    uint8_t buffer[2] = {reg, val};
    HAL_StatusTypeDef status;

    status = HAL_I2C_Master_Transmit(sp_hi2c, SLAVE_ADDR, buffer, 2, 100);

    // SCCB needs ACK after reg, before data
    if(status == HAL_OK) HAL_Delay(1);  // Critical timing

}

static void ov7670_read(uint8_t regAddr, uint8_t *data)
{
  HAL_StatusTypeDef ret;
  do {

    ret = HAL_I2C_Master_Transmit(sp_hi2c, SLAVE_ADDR, &regAddr, 1, 100);
    ret |= HAL_I2C_Master_Receive(sp_hi2c, SLAVE_ADDR, data, 1, 100);
  } while (ret != HAL_OK && 0);
}
