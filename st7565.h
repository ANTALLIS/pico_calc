#pragma once
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <string.h>

#define SPI_PORT spi0
#define PIN_CS 1
#define PIN_RST 5
#define PIN_DC 4
#define PIN_SCK 2
#define PIN_MOSI 3

#define LCD_WIDTH 128
#define LCD_HEIGHT 64
#define LCD_PAGES (LCD_HEIGHT / 8)

void lcd_init(void);
void lcd_clear(void);
void lcd_fill(void);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_set_page(uint8_t page);
void lcd_set_column(uint8_t col);
void lcd_update(void);
void lcd_draw_pixel(int x, int y, bool on);

extern uint8_t framebuffer[LCD_PAGES][LCD_WIDTH];
