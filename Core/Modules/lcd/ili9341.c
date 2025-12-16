/*
 * ili9341.c
 *
 *  Created on: Dec 4, 2025
 *      Author: Nam Truong
 */
#include "lcd/fonts.h"
#include "lcd/ili9341.h"
#include "lcd/ili9341_config.h"
#include <stdio.h>
#include "main.h"
#include "stm32f4xx_hal.h"


/*** Internal Function Declarations ***/
static void write_data(uint16_t data);
static void write_cmd(uint16_t cmd);
static uint16_t read_data();

/*** External Function Defines ***/
void lcd_ILI_init()
{

	HAL_GPIO_WritePin(FSMC_BLK_GPIO_Port, FSMC_BLK_Pin, GPIO_PIN_SET);HAL_Delay(10);
	HAL_GPIO_WritePin(FSMC_RES_GPIO_Port, FSMC_RES_Pin, GPIO_PIN_SET);HAL_Delay(10);
	HAL_GPIO_WritePin(FSMC_RES_GPIO_Port, FSMC_RES_Pin, GPIO_PIN_RESET);HAL_Delay(10);
	HAL_GPIO_WritePin(FSMC_RES_GPIO_Port, FSMC_RES_Pin, GPIO_PIN_SET);HAL_Delay(10);

	write_cmd(0x01); //software reset
	HAL_Delay(50);
	write_cmd(0x11); //exit sleep
	HAL_Delay(50);

	write_cmd(0xB6);
	write_data(0x0A);
	write_data(0xC2);

	write_cmd(0x36);   // memory access control
	write_data(0xc8);
	//  write_data(0x60);     // RGB -> seems BGR

	write_cmd(0x3A); // pixel format
	write_data(0x55); //RGB565 (16bit)

	write_cmd(0xE0); //gamma
	write_data(0x10);
	write_data(0x10);
	write_data(0x10);
	write_data(0x08);
	write_data(0x0E);
	write_data(0x06);
	write_data(0x42);
	write_data(0x28);
	write_data(0x36);
	write_data(0x03);
	write_data(0x0E);
	write_data(0x04);
	write_data(0x13);
	write_data(0x0E);
	write_data(0x0C);

	write_cmd(0XE1); //gamma
	write_data(0x0C);
	write_data(0x23);
	write_data(0x26);
	write_data(0x04);
	write_data(0x0C);
	write_data(0x04);
	write_data(0x39);
	write_data(0x24);
	write_data(0x4B);
	write_data(0x03);
	write_data(0x0B);
	write_data(0x0B);
	write_data(0x33);
	write_data(0x37);
	write_data(0x0F);

	write_cmd(0x2a);//
	write_data(0x00);
	write_data(0x00);
	write_data(0x00);
	write_data(0xef);

	write_cmd(0x2b); //
	write_data(0x00);
	write_data(0x00);
	write_data(0x01);
	write_data(0x3f);

	write_cmd(0x29);
	HAL_Delay(10);
	write_cmd(0x2C);

	lcd_ILI_draw_rect(0, 0, LCD_ILI_WIDTH, LCD_ILI_HEIGHT, COLOR_BLACK);
	lcd_ILI_set_write_area(0, 0, LCD_ILI_WIDTH - 1, LCD_ILI_HEIGHT - 1);

}

void lcd_ILI_set_write_area(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
	write_cmd(0x2a);
	write_data(xStart >> 8);
	write_data(xStart & 0xff);
	write_data(xEnd >> 8);
	write_data(xEnd & 0xff);

	write_cmd(0x2b);
	write_data(yStart >> 8);
	write_data(yStart & 0xff);
	write_data(yEnd >> 8);
	write_data(yEnd & 0xff);

	write_cmd(0x2c); // set cmd for writing
}

void lcd_ILI_set_read_area(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
	write_cmd(0x2a);
	write_data(xStart >> 8);
	write_data(xStart & 0xff);
	write_data(xEnd >> 8);
	write_data(xEnd & 0xff);

	write_cmd(0x2b);
	write_data(yStart >> 8);
	write_data(yStart & 0xff);
	write_data(yEnd >> 8);
	write_data(yEnd & 0xff);

	write_cmd(0x2e); // set cmd for reading

	// the first read is invalid
	read_data();
}

void lcd_ILI_draw_rect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color)
{
	lcd_ILI_set_write_area(xStart, yStart, xStart + width - 1, yStart + height - 1);
	for( uint16_t y = 0; y < height; y++ ){
		for( uint16_t x = 0; x < width; x++ ){
		//      write_data(color >> 8);
		//      write_data(color);
		  LCD_DATA = color;
		}
	}
}

/* string displaying */
/* Draw a single 8x8 character (optimized) */
void lcd_ILI_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg)
{
    if(c < 32 || c > 127) return; // only printable
    c -= 32; // index into font array

    // Set the write area for the full 8x8 block
    lcd_ILI_set_write_area(x, y, x + 7, y + 7);

    // Write pixels sequentially row by row
    for(uint8_t row = 0; row < 8; row++)
    {
        uint8_t bits = font8x8_basic[(uint8_t)c][row];

        for(uint8_t col = 0; col < 8; col++)
        {
            uint16_t pixel_color = (bits & (1 << (7 - col))) ? color : bg;
            LCD_DATA = pixel_color;
        }
    }
}

/* Draw a string of characters */
void lcd_ILI_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg)
{
    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    while(*str)
    {
        if(*str == '\n')
        {
            cursor_x = x;
            cursor_y += 8; // next line
        }
        else
        {
            // Wrap line if needed
            if(cursor_x + 8 > LCD_ILI_WIDTH)
            {
                cursor_x = x;
                cursor_y += 8;
            }

            lcd_ILI_draw_char(cursor_x, cursor_y, *str, color, bg);
            cursor_x += 8; // move to next character
        }
        str++;
    }
}



/* frame display at the center of lcd */
void lcd_ILI_display_frame(uint16_t *buf, uint16_t buf_width, uint16_t buf_height)
{
    // Center the frame on the LCD
    uint16_t xStart = (LCD_ILI_WIDTH - buf_width) / 2;
    uint16_t yStart = (LCD_ILI_HEIGHT - buf_height) / 2;
    uint16_t xEnd   = xStart + buf_width - 1;
    uint16_t yEnd   = yStart + buf_height - 1;

    // Set write window
    lcd_ILI_set_write_area(xStart, yStart, xEnd, yEnd);

    // Copy the buffer to LCD GRAM
    for (uint32_t i = 0; i < buf_width * buf_height; i++)
    {
        LCD_DATA = buf[i];
    }
}


// fpga comm side
uint16_t* lcd_ILI_get_draw_addr()
{
	lcd_ILI_set_write_area(0, 0, LCD_ILI_WIDTH - 1, LCD_ILI_HEIGHT - 1);
	return (uint16_t*)LCD_DATA_ADDR;
}

void lcd_ILI_get_subframe_RGB565(uint16_t *buf, uint16_t w, uint16_t h)
{
    uint16_t x0 = (LCD_ILI_WIDTH  - w) / 2;
    uint16_t y0 = (LCD_ILI_HEIGHT - h) / 2;

    uint32_t idx = 0;

        lcd_ILI_set_read_area(
            x0, y0,
            x0 + w - 1,
			y0 + h - 1
        );


        for (uint16_t i = 0; i < w*h/2; i++)
        {
            /* RGB666 read (3 reads per pixel) */
            uint16_t d0 = read_data();
            uint16_t d1 = read_data();
            uint16_t d2 = read_data();

            uint8_t r1 = d0 >> 8;
            uint8_t g1 = d0 & 0xFF;
            uint8_t b1 = d1 >> 8;

            uint8_t r2 = d1 & 0xFF;
            uint8_t g2 = d2 >> 8;
            uint8_t b2 = d2 & 0xFF;


            buf[idx++] =
                ((r1 & 0xF8) << 8) |
                ((g1 & 0xFC) << 3) |
                (b1 >> 3);

            buf[idx++] =
                ((r2 & 0xF8) << 8) |
                ((g2 & 0xFC) << 3) |
                (b2 >> 3);
        }
}

void lcd_ILI_get_subframe_GreyScale(uint8_t *buf, uint16_t w, uint16_t h)
{
	uint16_t x0 = (LCD_ILI_WIDTH  - w) / 2;
	uint16_t y0 = (LCD_ILI_HEIGHT - h) / 2;

	uint32_t idx = 0;

	lcd_ILI_set_read_area(
		x0, y0,
		x0 + w - 1,
		y0 + h - 1
	);

	for (uint32_t i = 0; i < (w * h) / 2; i++)
	{
		uint16_t d0 = read_data();
		uint16_t d1 = read_data();
		uint16_t d2 = read_data();

		// unpack RGB666 → RGB888 (2 pixels)
		uint8_t r1 = d0 >> 8;
		uint8_t g1 = d0 & 0xFF;
		uint8_t b1 = d1 >> 8;

		uint8_t r2 = d1 & 0xFF;
		uint8_t g2 = d2 >> 8;
		uint8_t b2 = d2 & 0xFF;

		// luminance
		uint8_t pix1 = (77*r1 + 150*g1 + 29*b1) >> 8;
		uint8_t pix2 = (77*r2 + 150*g2 + 29*b2) >> 8;

		// pack back to RGB565 (R=G=B=Y)
		buf[idx++] =
			((pix1 & 0xF8) << 8) |
			((pix1 & 0xFC) << 3) |
			(pix1 >> 3);

		buf[idx++] =
			((pix2 & 0xF8) << 8) |
			((pix2 & 0xFC) << 3) |
			(pix2 >> 3);
	}
}


/*** Internal Function Defines ***/
inline static void write_cmd(uint16_t cmd)
{
	LCD_CMD = cmd;
}

inline static void write_data(uint16_t data)
{
	LCD_DATA = data;
}

inline static uint16_t read_data()
{
	uint16_t data = LCD_DATA;
	return data;
}



