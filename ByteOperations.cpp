/**
 * @file    ByteOperations.cpp
 * @brief   Basic word/halfword to byte array conversion
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @details	The functions are desiged to be used in sequential order. \n
 *					With the use of external array index pointer \n
 *					multiple functions can be called at any place in the array.
 *
 * @{
 */
 
#include "ByteOperations.h"
#include <stdint.h>

/**
 * @brief   Write 32 Bit word to 4 bytes
 * @note		Lower index equals least significant byte 
 *
 * @param[in,out] &i		Index variable pointer, will incement 4
 * @param[out] 		dst		Destination array
 * @param[in] 		src	  Input variable
 */
void write_32_to_8(int *i, uint8_t dst[], uint32_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
	dst[(*i)++] = (src >> 16)&0xFF;
	dst[(*i)++] = (src >> 24)&0xFF;
}

/**
 * @brief   Write 16 Bit word to 2 bytes
 * @note		Lower index equals least significant byte 
 *
 * @param[in,out] &i		Index variable pointer will incement 2
 * @param[out] 		dst		Destination array
 * @param[in] 		src	  Input variable
 */
void write_16_to_8(int *i, uint8_t dst[], uint16_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
}

/**
 * @brief   Read 32 Bit word from 4 bytes
 * @note		Lower index equals least significant byte 
 *
 * @param[in,out] &i		Index variable pointer will incement 4
 * @param[in] 		src	  Source array
 * @return				Read 	value
 */
uint32_t read_8_to_32(int *i, uint8_t *src){
	uint32_t data = 0;
	data |= (src[(*i)++] << 0) ; 
	data |= (src[(*i)++] << 8) ; 
	data |= (src[(*i)++] << 16); 
	data |= (src[(*i)++] << 24); 
	return data;
}

/**
 * @brief   Read 16 Bit word from 2 bytes
 * @note		Lower index equals least significant byte 
 *
 * @param[in,out] &i		Index variable pointer, will incement 2
 * @param[in] 		src		Direction to step in, 1 or 0
 * @return				Read value
 */
uint16_t read_8_to_16(int *i, uint8_t *src){
	uint16_t data = 0;
	data |= (src[(*i)++] << 0) ;
	data |= (src[(*i)++] << 8) ;
	return data;
}
/**
 * @}
 */
