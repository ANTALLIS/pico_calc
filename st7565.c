#include "st7565.h"

uint8_t framebuffer[LCD_PAGES][LCD_WIDTH];

static void cs_select() { gpio_put(PIN_CS, 0); }
static void cs_deselect() { gpio_put(PIN_CS, 1); }

void lcd_send_command(uint8_t cmd) {
  gpio_put(PIN_DC, 0);
  cs_select();
  spi_write_blocking(SPI_PORT, &cmd, 1);
  cs_deselect();
}

void lcd_send_data(uint8_t data) {
  gpio_put(PIN_DC, 1);
  cs_select();
  spi_write_blocking(SPI_PORT, &data, 1);
  cs_deselect();
}

void lcd_init(void) {
  spi_init(SPI_PORT, 400000);
  spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  gpio_init(PIN_CS);
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);
  gpio_init(PIN_RST);
  gpio_set_dir(PIN_RST, GPIO_OUT);
  gpio_init(PIN_DC);
  gpio_set_dir(PIN_DC, GPIO_OUT);

  gpio_put(PIN_RST, 0);
  sleep_ms(100);
  gpio_put(PIN_RST, 1);
  sleep_ms(100);

  lcd_send_command(0xA2);
  lcd_send_command(0xA1);
  lcd_send_command(0xC8);
  lcd_send_command(0x40);
  lcd_send_command(0x26);
  lcd_send_command(0x81);
  lcd_send_command(0x20);
  lcd_send_command(0x2F);
  lcd_send_command(0xA6);
  lcd_send_command(0xAF);

  lcd_clear();
  lcd_update();
}

void lcd_clear(void) { memset(framebuffer, 0, sizeof(framebuffer)); }

void lcd_fill(void) { memset(framebuffer, 0xFF, sizeof(framebuffer)); }

void lcd_set_page(uint8_t page) { lcd_send_command(0xB0 | (page & 0x0F)); }

void lcd_set_column(uint8_t col) {
  col += 4;
  lcd_send_command(0x10 | (col >> 4));
  lcd_send_command(0x00 | (col & 0x0F));
}

void lcd_update(void) {
  for (uint8_t page = 0; page < LCD_PAGES; page++) {
    lcd_set_page(page);
    lcd_set_column(0);
    for (uint8_t col = 0; col < LCD_WIDTH; col++) {
      lcd_send_data(framebuffer[page][col]);
    }
  }
}

void lcd_draw_pixel(int x, int y, bool on) {
  if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT)
    return;
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  if (on)
    framebuffer[page][x] |= (1 << bit);
  else
    framebuffer[page][x] &= ~(1 << bit);
}
