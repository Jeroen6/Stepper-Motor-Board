#ifndef BYTEOPERATION_H_
#define BYTEOPERATION_H_

#include <stdint.h>

/* low index equals low nibble */
/* Write 32 bit var to 4 bytes */
void write_32_to_8(int *i, uint8_t dst[], uint32_t src);

/* Write 16 bit var to 2 bytes */
void write_16_to_8(int *i, uint8_t dst[], uint16_t src);

/* Write 4 bytes to 32 bit var*/
uint32_t read_8_to_32(int *i, uint8_t *src);

/* Write 2 bytes to 16 bit var*/
uint16_t read_8_to_16(int *i, uint8_t *src);

#endif
