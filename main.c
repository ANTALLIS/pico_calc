#include "pico/bootrom.h"
#include "pico/stdio_usb.h" // for stdio_usb_connected()
#include "pico/stdlib.h"
#include "st7565.h"
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "font8x8_basic.h"

// Define your matrix pins - adjust these to match your wiring
#define ROWS 4
#define COLS 5

uint row_pins[ROWS] = {16, 17, 18, 19};     // adjust to your row pins
uint col_pins[COLS] = {15, 14, 13, 12, 11}; // adjust to your col pins

bool keys[4][5] = {
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
    {false, false, false, false, false},
};

void init_matrix(void) {
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

void get_held_keys(bool key_array[4][5]) {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      key_array[i][j] = false;
    }
  }

  for (int r = 0; r < ROWS; r++) {
    // Pull row low
    gpio_put(row_pins[r], 0);
    sleep_us(10); // settle time

    for (int c = 0; c < COLS; c++) {
      if (!gpio_get(col_pins[c])) { // low = pressed (pull-up)
        key_array[r][c] = true;
      }
    }

    // Restore row high
    gpio_put(row_pins[r], 1);
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

void draw_no_no_text(int x, int y) {
  // clang-format off
  char space_pixels[] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
  };
  char n_pixels[] = {
    1, 0, 0, 1,
    1, 0, 0, 1,
    1, 1, 0, 1,
    1, 0, 1, 1,
    1, 0, 0, 1,
  };
  char o_pixels[] = {
    0, 1, 1, 0,
    1, 0, 0, 1,
    1, 0, 0, 1,
    1, 0, 0, 1,
    0, 1, 1, 0,
  };
  char s_pixels[] = {
    0, 1, 1, 1,
    1, 0, 0, 0,
    0, 1, 1, 0,
    0, 0, 0, 1,
    1, 1, 1, 0,
  };
  char q_pixels[] = {
    0, 1, 1, 0,
    1, 0, 0, 1,
    1, 1, 0, 1,
    1, 0, 1, 0,
    0, 1, 0, 1,
  };
  char u_pixels[] = {
    1, 0, 0, 1,
    1, 0, 0, 1,
    1, 0, 0, 1,
    1, 0, 0, 1,
    0, 1, 1, 0,
  };
  char a_pixels[] = {
    0, 1, 1, 0,
    1, 0, 0, 1,
    1, 1, 1, 1,
    1, 0, 0, 1,
    1, 0, 0, 1,
  };
  char r_pixels[] = {
    1, 1, 1, 0,
    1, 0, 0, 1,
    1, 1, 1, 1,
    1, 0, 1, 0,
    1, 0, 0, 1,
  };
  char e_pixels[] = {
    1, 1, 1, 1,
    1, 0, 0, 0,
    1, 1, 1, 1,
    1, 0, 0, 0,
    1, 1, 1, 1,
  };
  // clang-format on

  char *my_letters[] = {
      n_pixels, o_pixels, space_pixels, n_pixels, o_pixels, space_pixels,
      s_pixels, q_pixels, u_pixels,     a_pixels, r_pixels, e_pixels,
  };
  for (int z = 0; z < 12; z++) {
    int offset = 5 * z;
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        int idx = i * 4 + j;
        bool to_draw = my_letters[z][idx];
        lcd_draw_pixel((x + j + offset), y + i, to_draw);
      }
    }
  }
}

void draw_text(char *text, int x, int y) {
  size_t len = strlen(text);
  for (int i = 0; i < len; i++) {
    char letter = text[i];
    char *glyph = font8x8_basic[letter];
    int offset = 8 * i;
    for (int dy = 0; dy < 8; dy++) {
      for (int dx = 0; dx < 8; dx++) {
        bool to_draw = (glyph[dy] >> dx) & 0x01;
        lcd_draw_pixel((x + dx + offset), y + dy, to_draw);
      }
    }
  }
}

bool is_key_pressed(void) {
  bool ret_value = false;
  for (int r = 0; r < ROWS; r++) {
    // Pull row low
    gpio_put(row_pins[r], 0);
    sleep_us(10); // settle time

    for (int c = 0; c < COLS; c++) {
      if (!gpio_get(col_pins[c])) { // low = pressed (pull-up)
        ret_value = true;
      }
    }

    // Restore row high
    gpio_put(row_pins[r], 1);
  }
  return ret_value;
}

typedef enum {
  Key_None,
  Key_0,
  Key_1,
  Key_2,
  Key_3,
  Key_4,
  Key_5,
  Key_6,
  Key_7,
  Key_8,
  Key_9,
  Key_Add,
  Key_Sub,
  Key_Mult,
  Key_Div,
  Key_Period,
  Key_Equal,
  Key_Enter,
  Key_Shift,
  Key_Intro,
  Key_Elim,
  Key_Del,
  Key_Clear,
  Key_Bra,
  Key_Ket,
  Key_Undo,
  Key_Or,
  Key_And,
  Key_Not,
  Key_Implies,
  Key_All,
  Key_Exists,
  Key_Push,
  Key_Pop,
} KeyCode;

int get_keycode(void) {
  get_held_keys(keys);
  KeyCode ret_val = Key_None;
  if (keys[0][1]) {
    ret_val = Key_0;
  } else if (keys[1][0]) {
    ret_val = Key_1;
  } else if (keys[1][1]) {
    ret_val = Key_2;
  } else if (keys[1][2]) {
    ret_val = Key_3;
  } else if (keys[2][0]) {
    ret_val = Key_4;
  } else if (keys[2][1]) {
    ret_val = Key_5;
  } else if (keys[2][2]) {
    ret_val = Key_6;
  } else if (keys[3][0]) {
    ret_val = Key_7;
  } else if (keys[3][1]) {
    ret_val = Key_8;
  } else if (keys[3][2]) {
    ret_val = Key_9;
  } else if (keys[0][1]) {
    ret_val = Key_Period;
  } else if (keys[0][3]) {
    ret_val = Key_Add;
  } else if (keys[1][3]) {
    ret_val = Key_Mult;
  } else if (keys[2][3]) {
    ret_val = Key_Sub;
  } else if (keys[3][3]) {
    ret_val = Key_Div;
  }
  while (is_key_pressed()) {
    sleep_ms(10);
  }
  return ret_val;
}

bool is_key_digit(KeyCode k) {
  switch (k) {
  case Key_0:
  case Key_1:
  case Key_2:
  case Key_3:
  case Key_4:
  case Key_5:
  case Key_6:
  case Key_7:
  case Key_8:
  case Key_9:
    return true;
  default:
    return false;
  }
}

int key_to_digit(KeyCode k) {
  switch (k) {
  case Key_0:
    return 0;
  case Key_1:
    return 1;
  case Key_2:
    return 2;
  case Key_3:
    return 3;
  case Key_4:
    return 4;
  case Key_5:
    return 5;
  case Key_6:
    return 6;
  case Key_7:
    return 7;
  case Key_8:
    return 8;
  case Key_9:
    return 9;
  default:
    return 0;
  }
}

/*
 * Get key press
 * Add to count
 * Turn total into string
 * Show string on display
 */

int get_digit(void) {
  get_held_keys(keys);
  int ret_val = -1;
  if (keys[0][0]) {
    ret_val = 0;
  } else if (keys[1][0]) {
    ret_val = 1;
  } else if (keys[1][1]) {
    ret_val = 2;
  } else if (keys[1][2]) {
    ret_val = 3;
  } else if (keys[2][0]) {
    ret_val = 4;
  } else if (keys[2][1]) {
    ret_val = 5;
  } else if (keys[2][2]) {
    ret_val = 6;
  } else if (keys[3][0]) {
    ret_val = 7;
  } else if (keys[3][1]) {
    ret_val = 8;
  } else if (keys[3][2]) {
    ret_val = 9;
  }
  while (is_key_pressed()) {
    sleep_ms(10);
  }
  return ret_val;
}

void update_count_text(char *buf, size_t max_len, int num) {
  snprintf(buf, 50, "%d", num);
  draw_text(buf, 32, 50);
}
void update_total_text(char *buf, size_t max_len, int num) {
  snprintf(buf, 50, "%d", num);
  draw_text(buf, 32, 10);
}

void calc_mode(void) {
  int MAX_NUMBER = 9999999;
  int total = 0;
  int digit;
  int number_buffer = 0;
  int new_number_buffer = 0;
  char text_buffer[51];
  char total_buffer[51];

  int *my_mem = malloc(500 * sizeof(int));
  assert(my_mem != NULL);
  my_mem[50] = 100;
  free(my_mem);

  // printf("%s\n", text_buffer);
  lcd_clear();
  update_count_text(text_buffer, 50, number_buffer);
  update_total_text(total_buffer, 50, total);
  draw_text("[\x08 \x09 \x0A \x0B \x07\x06]", 12, 40);
  lcd_update();
  while (true) {
    sleep_ms(50);
    if (is_key_pressed()) {
      KeyCode key = get_keycode();
      digit = key_to_digit(key);
      if (key == Key_Add) {
        total += number_buffer;
        number_buffer = 0;
        snprintf(text_buffer, 50, "0");
      } else if (key == Key_Sub) {
        total -= number_buffer;
        number_buffer = 0;
        snprintf(text_buffer, 50, "0");
      } else if (key == Key_Mult) {
        total *= number_buffer;
        number_buffer = 0;
        snprintf(text_buffer, 50, "0");
      } else if (key == Key_Div) {
        if (number_buffer == 0) {
          lcd_clear();
          draw_text("Cannot divide", 12, 40);
          draw_text(" by zero.", 12, 50);
          update_total_text(total_buffer, 50, total);
          lcd_update();
          continue;
        }
        total /= number_buffer;
        number_buffer = 0;
        snprintf(text_buffer, 50, "0");
      } else if (!is_key_digit(key)) {
        continue;
      }
    } else {
      continue;
    }
    lcd_clear();
    new_number_buffer = number_buffer;
    new_number_buffer *= 10;
    new_number_buffer += digit;
    if (new_number_buffer > MAX_NUMBER) {
      continue;
    }
    number_buffer = new_number_buffer;
    update_count_text(text_buffer, 50, number_buffer);
    update_total_text(total_buffer, 50, total);
    lcd_update();
  }
}

int main(void) {
  stdio_init_all();
  init_matrix();
  lcd_init();
  // while (!stdio_usb_connected()) {
  //   sleep_ms(100);
  // }

  calc_mode();
  while (true) {
    lcd_clear();
    if (keys[1][1]) {
      draw_box();
    }
    if (keys[1][2]) {
      draw_no_no_text(32, 30);
    }
    draw_text("Hewwo :3", 32, 40);
    draw_text("0 != 0", 32, 50);
    lcd_update();
    get_held_keys(keys);

    sleep_ms(50);
  }
  reset_usb_boot(0, 0);
}
