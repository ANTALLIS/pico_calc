#include "pico/bootrom.h"
#include "pico/stdio_usb.h" // for stdio_usb_connected()
#include "pico/stdlib.h"
#include "st7565.h"
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdio.h>

// Define your matrix pins - adjust these to match your wiring
#define ROWS 4
#define COLS 5

uint row_pins[ROWS] = {16, 17, 18, 19};     // adjust to your row pins
uint col_pins[COLS] = {11, 12, 13, 14, 15}; // adjust to your col pins

void init_matrix() {
  // Rows as outputs
  for (int r = 0; r < ROWS; r++) {
    gpio_init(row_pins[r]);
    gpio_set_dir(row_pins[r], GPIO_OUT);
    gpio_put(row_pins[r], 1); // default high
  }

  // Cols as inputs with pull-ups
  for (int c = 0; c < COLS; c++) {
    gpio_init(col_pins[c]);
    gpio_set_dir(col_pins[c], GPIO_IN);
    gpio_pull_up(col_pins[c]);
  }
}

// Test 1: blink each row pin so you can probe with multimeter
void test_row_pins() {
  printf("\n=== ROW PIN BLINK TEST ===\n");
  printf("Each row pin will go LOW for 2 seconds. Probe with multimeter.\n\n");

  for (int r = 0; r < ROWS; r++) {
    printf("Row %d (GPIO %d) going LOW now...\n", r, row_pins[r]);
    gpio_put(row_pins[r], 0);
    sleep_ms(2000);
    gpio_put(row_pins[r], 1);
    printf("Row %d done\n\n", r);
    sleep_ms(500);
  }
}

// Test 2: manually short a col to GND and see if it registers
void test_col_pins() {
  printf("\n=== COL PIN INPUT TEST ===\n");
  printf("Manually short each col pin to GND and watch for detection.\n\n");

  for (int c = 0; c < COLS; c++) {
    printf("Watching COL %d (GPIO %d) for 3 seconds - short it to GND now...\n",
           c, col_pins[c]);
    uint32_t start = to_ms_since_boot(get_absolute_time());
    bool detected = false;
    while (to_ms_since_boot(get_absolute_time()) - start < 3000) {
      if (!gpio_get(col_pins[c])) {
        printf("  DETECTED on COL %d (GPIO %d)!\n", c, col_pins[c]);
        detected = true;
        sleep_ms(200); // debounce
      }
    }
    if (!detected)
      printf("  Nothing detected on COL %d\n", c);
  }
}

// Test 3: read all pins raw and dump state
void dump_pin_state() {
  printf("\n=== RAW PIN STATE ===\n");
  printf("Row outputs:  ");
  for (int r = 0; r < ROWS; r++) {
    printf("GPIO%d=%d  ", row_pins[r], gpio_get(row_pins[r]));
  }
  printf("\nCol inputs:   ");
  for (int c = 0; c < COLS; c++) {
    printf("GPIO%d=%d  ", col_pins[c], gpio_get(col_pins[c]));
  }
  printf("\n");
}

void scan_matrix() {
  for (int r = 0; r < ROWS; r++) {
    // Pull row low
    gpio_put(row_pins[r], 0);
    sleep_us(10); // settle time

    for (int c = 0; c < COLS; c++) {
      if (!gpio_get(col_pins[c])) { // low = pressed (pull-up)
        printf("Key pressed at ROW %d, COL %d (GPIO %d, GPIO %d)\n", r, c,
               row_pins[r], col_pins[c]);
      }
    }

    // Restore row high
    gpio_put(row_pins[r], 1);
  }
}

void show_keys(void) {
  bool print_keys = false;
  bool keys[4][5] = {
      {false, false, false, false, false},
      {false, false, false, false, false},
      {false, false, false, false, false},
      {false, false, false, false, false},
  };
  for (int r = 0; r < ROWS; r++) {
    // Pull row low
    gpio_put(row_pins[r], 0);
    sleep_us(10); // settle time

    for (int c = 0; c < COLS; c++) {
      if (!gpio_get(col_pins[c])) { // low = pressed (pull-up)
        keys[r][c] = true;
        print_keys = true;
      }
    }

    // Restore row high
    gpio_put(row_pins[r], 1);
  }
  if (print_keys) {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        char key_char = 'O';
        if (keys[ROWS - r - 1][COLS - c - 1]) {
          key_char = 'x';
        }
        printf("%c", key_char);
      }
      printf("\n");
    }
    printf("\n\n\n");
  }
}

#define BOX_SIZE 40

void draw_box(void) {
  // Draw a hollow square (40x40, centered)
  int x = (LCD_WIDTH - 40) / 2;  // 44
  int y = (LCD_HEIGHT - 40) / 2; // 12

  // Top and bottom edges
  for (int i = x; i < x + 40; i++) {
    lcd_draw_pixel(i, y, true);
    lcd_draw_pixel(i, y + 39, true);
  }
  // Left and right edges
  for (int j = y; j < y + 40; j++) {
    lcd_draw_pixel(x, j, true);
    lcd_draw_pixel(x + 39, j, true);
  }
}

void draw_rectangle(void) {
  lcd_clear();
  draw_box();
  lcd_update();
}

void draw_box_new(int x, int y) {
  for (int i = x; i < x + BOX_SIZE; i++) {
    lcd_draw_pixel(i, y, true);
    lcd_draw_pixel(i, y + BOX_SIZE - 1, true);
  }
  for (int j = y; j < y + BOX_SIZE; j++) {
    lcd_draw_pixel(x, j, true);
    lcd_draw_pixel(x + BOX_SIZE - 1, j, true);
  }
}

int moving_box(void) {
  stdio_init_all();
  lcd_init();

  int x = 0, y = 0;
  int dx = 4, dy = 4;

  while (true) {
    lcd_clear();
    draw_box_new(x, y);
    lcd_update();

    x += dx;
    y += dy;

    if (x <= 0 || x >= LCD_WIDTH - BOX_SIZE)
      dx = -dx;
    if (y <= 0 || y >= LCD_HEIGHT - BOX_SIZE)
      dy = -dy;

    sleep_ms(64); // ~60fps
  }
}

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    sleep_ms(100);

  printf("Matrix scanner ready\n");
  init_matrix();

  printf("=== MATRIX DIAGNOSTIC ===\n");
  printf("Commands:\n");
  printf("  '1' = blink each row pin (use multimeter)\n");
  printf("  '2' = test col inputs (short to GND)\n");
  printf("  '3' = dump raw pin state\n");
  printf("  '4' = scan matrix\n");
  printf("  '5' = draw rectangle\n");
  printf("  '6' = draw moving rectangle\n");
  printf("  'r' = reboot to bootloader\n\n");

  init_matrix();

  while (true) {
    int c = getchar_timeout_us(100000);
    switch (c) {
    case '1':
      test_row_pins();
      break;
    case '2':
      test_col_pins();
      break;
    case '3':
      dump_pin_state();
      break;
    case '4':
      while (true) {
        int ch = getchar_timeout_us(0);
        if (ch == 'r')
          reset_usb_boot(0, 0);

        // scan_matrix();
        show_keys();
        sleep_ms(50); // debounce
      }
      break;
    case '5':
      stdio_init_all();
      lcd_init();
      while (true) {
        int ch = getchar_timeout_us(0);
        if (ch == 'r')
          reset_usb_boot(0, 0);
        draw_rectangle();
        sleep_ms(50);
      }
      break;
    case '6':
      moving_box();
      break;
    case 'r':
      reset_usb_boot(0, 0);
      break;
    }
  }
}
