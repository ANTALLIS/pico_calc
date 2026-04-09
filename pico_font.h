#include <stdint.h>

#define CHARACTER_WIDTH 4
#define CHARACTER_HEIGHT 5

// clang-format off
uint8_t space_pixels[] = {
  0b0000,
  0b0000,
  0b0000,
  0b0000,
  0b0000,
};
uint8_t fill_pixels[] = {
  0b1111,
  0b1111,
  0b1111,
  0b1111,
  0b1111,
};
uint8_t n_pixels[] = {
  0b1001,
  0b1001,
  0b1101,
  0b1011,
  0b1001,
};
uint8_t o_pixels[] = {
  0b0110,
  0b1001,
  0b1001,
  0b1001,
  0b0110,
};
uint8_t s_pixels[] = {
  0b0111,
  0b1000,
  0b0110,
  0b0001,
  0b1110,
};
uint8_t q_pixels[] = {
  0b0110,
  0b1001,
  0b1101,
  0b1010,
  0b0101,
};
uint8_t u_pixels[] = {
  0b1001,
  0b1001,
  0b1001,
  0b1001,
  0b0110,
};
uint8_t a_pixels[] = {
  0b0110,
  0b1001,
  0b1111,
  0b1001,
  0b1001,
};
uint8_t r_pixels[] = {
  0b1110,
  0b1001,
  0b1111,
  0b1010,
  0b1001,
};
uint8_t e_pixels[] = {
  0b1111,
  0b1000,
  0b1111,
  0b1000,
  0b1111,
};
uint8_t exists_pixels[] = {
  0b1111,
  0b0001,
  0b1111,
  0b0001,
  0b1111,
};
uint8_t arrow_pixels[] = {
  0b0100,
  0b0010,
  0b1111,
  0b0010,
  0b0100,
};
// clang-format on

uint8_t *my_letters[] = {
    n_pixels, o_pixels, space_pixels, n_pixels, o_pixels, space_pixels,
    s_pixels, q_pixels, u_pixels,     a_pixels, r_pixels, e_pixels,
};
