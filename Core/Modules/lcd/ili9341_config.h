/*
 * ili9341_config.h
 *
 *  Created on: Dec 14, 2025
 *      Author: Nam Truong
 */

#ifndef MODULES_LCD_ILI9341_CONFIG_H_
#define MODULES_LCD_ILI9341_CONFIG_H_


#define BIT_WIDTH_16
#define FSMC_Ax               17              // use A17 as RS
#define FSMC_NEx              1               // use subbank 1

#define LCD_ILI_RED    0xf800
#define LCD_ILI_GREEN  0x07e0
#define LCD_ILI_BLUE   0x001f

#define QQVGA

#ifdef QVGA
#define LCD_ILI_WIDTH  320
#define LCD_ILI_HEIGHT 240
#endif

#ifdef QQVGA
#define LCD_ILI_WIDTH  160
#define LCD_ILI_HEIGHT 120
#endif

/*** ADDRESSES ***/
#define FSMC_ADDRESS  (0x60000000 + ((FSMC_NEx-1) << 26))
#define LCD_CMD_ADDR  (FSMC_ADDRESS)

#define LCD_DATA_ADDR (FSMC_ADDRESS | 1 << (FSMC_Ax + 1))
#define LCD_CMD       (*((volatile uint16_t*) LCD_CMD_ADDR))
#define LCD_DATA      (*((volatile uint16_t*) LCD_DATA_ADDR))


#endif /* MODULES_LCD_ILI9341_CONFIG_H_ */
