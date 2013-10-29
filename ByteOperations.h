/**
 * @file    ByteOperations.cpp
 * @brief   Basic word/halfword to byte array conversion
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @{
 */
#ifndef BYTEOPERATION_H_
#define BYTEOPERATION_H_

#include <stdint.h>

void write_32_to_8(int *i, uint8_t dst[], uint32_t src);
void write_16_to_8(int *i, uint8_t dst[], uint16_t src);
uint32_t read_8_to_32(int *i, uint8_t *src);
uint16_t read_8_to_16(int *i, uint8_t *src);

#endif
/**
 * @}
 */
