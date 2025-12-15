/*
 * fonts.h
 *
 *  Created on: Dec 14, 2025
 *      Author: Nam Truong
 */

#ifndef MODULES_ILI9341_FONTS_H_
#define MODULES_ILI9341_FONTS_H_

#include <stdint.h>

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_GRAY      0x8410
#define COLOR_ORANGE    0xFD20

extern const uint8_t font8x8_basic[96][8];

#endif /* MODULES_ILI9341_FONTS_H_ */
