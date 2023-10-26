/**
 * Copyright (c) 2020 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include "../utils/utils.h"
#include <string.h>

#define B 32
uint32_t block[B];
uint32_t block2[B];

// addressing the bits while accounting for little endian
uint8_t bit_from_32(uint32_t *ptr, uint8_t i) {
  return (uint8_t)(((*ptr) >> ((i/8)*8 + 7-i%8)) & 0x1);
}

// addressing the bits while accounting for little endian
void set_bit_32(uint32_t *ptr, uint8_t i, uint8_t value) {
  uint32_t mask = (uint32_t) value;
  *ptr = (*ptr) | (mask << ((i/8)*8 + 7-i%8));
}

// Copy 32x32 block in the matrix into the buffer as the block's rotated version
void set_rotate_block(uint8_t *img, const bytes_t row_size, uint32_t i, uint32_t j, uint32_t *buf) {
  memset(buf, 0, B*sizeof(uint32_t));
  uint64_t byte_offset = j * row_size + (i / 8);
  uint8_t *img_byte = img + byte_offset;
  for (uint8_t k = 0; k < B; k++) {
    // Iterate through block row by row
    uint32_t *row = (uint32_t*) (img_byte + k * row_size);
    for (uint8_t l = 0; l < B; l++) {
      uint8_t bit = bit_from_32(row, l);
      set_bit_32(buf + l, (B-1) - k, bit);
    }
  }
}

// Copy the 32x32 block from the buffer back to the matrix
void set_block_back(uint8_t *img, const bytes_t row_size, uint32_t i, uint32_t j, uint32_t *buf) {
  uint64_t byte_offset = j * row_size + (i / 8);
  uint8_t *img_byte = img + byte_offset;
  for (uint8_t k = 0; k < B; k++) {
    uint32_t *row = (uint32_t*) (img_byte + k * row_size);
    *(row) = *(buf + k);
  }
}

// Rotates a bit array clockwise 90 degrees.
//
// The bit array is of `N` by `N` bits where N is a multiple of 64
void rotate_bit_matrix(uint8_t *img, const bits_t N) {
  // Get the number of bytes per row in `img`
  const uint32_t row_size = bits_to_bytes(N);

  uint32_t w, h;
  for (h = 0; h < N / 2; h+=B) {
    for (w = 0; w < N / 2; w+=B) {
      uint32_t i = w, j = h;
      // First Quadrant
      set_rotate_block(img, row_size, i, j, block);

      // Move a bit from one quadrant to the next and do this
      // for all 4 quadrants of the `img`
      uint32_t next_i = N - j - 1, next_j = i;
      set_rotate_block(img, row_size, next_i-B+1, next_j, block2);
      set_block_back(img, row_size, next_i-B+1, next_j, block);

      // Update the `i` and `j` with the next quadrant's values and
      // the `next_i` and `next_j` will get the new destination values
      i = next_i;
      j = next_j;

      // Second Quadrant
      next_i = N - j - 1, next_j = i;
      set_rotate_block(img, row_size, next_i-B+1, next_j-B+1, block);
      set_block_back(img, row_size, next_i-B+1, next_j-B+1, block2);

      i = next_i;
      j = next_j;

      // Third Quadrant
      next_i = N - j - 1, next_j = i;
      set_rotate_block(img, row_size, next_i, next_j-B+1, block2);
      set_block_back(img, row_size, next_i, next_j-B+1, block);

      i = next_i;
      j = next_j;

      // Fourth Quadrant
      next_i = N - j - 1, next_j = i;
      //printf("%d, %d\n", next_i, next_j);
      set_block_back(img, row_size, next_i, next_j, block2);

    }
  }
  return;
}
