/*
 * ili9341.h
 *
 *  Created on: Dec 4, 2025
 *      Author: Nam Truong
 */

#ifndef LCDILI9341_LCDILI9341_H_
#define LCDILI9341_LCDILI9341_H_

void lcd_ILI_init();
void lcd_ILI_set_write_area(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void lcd_ILI_set_read_area(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void lcd_ILI_draw_rect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color);

void lcd_ILI_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg);
void lcd_ILI_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg);

void lcd_ILI_display_frame(uint16_t *buf, uint16_t w, uint16_t h);

uint16_t* lcd_ILI_get_draw_addr();

void lcd_ILI_get_subframe_RGB565(uint16_t *rgb_buf, uint16_t w, uint16_t h);
void lcd_ILI_get_subframe_Grey(uint8_t *rgb_buf, uint16_t w, uint16_t h);
void lcd_ILI_display_frame_RGB565(uint16_t *gray_buf, uint16_t w, uint16_t h);
void lcd_ILI_display_frame_Grey(uint8_t *gray_buf, uint16_t w, uint16_t h);

void lcd_ILI_display_frame_RGB565_from_u32(
    uint32_t *buf,
    uint16_t buf_width,
    uint16_t buf_height
);
#endif /* LCDILI9341_LCDILI9341_H_ */
