#include "ByteOperations.h"
#include <stdint.h>


/* low index equals low nibble */
/* Write 32 bit var to 4 bytes */
void write_32_to_8(int *i, uint8_t dst[], uint32_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
	dst[(*i)++] = (src >> 16)&0xFF;
	dst[(*i)++] = (src >> 24)&0xFF;
}

/* Write 16 bit var to 2 bytes */
void write_16_to_8(int *i, uint8_t dst[], uint16_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
}

/* Write 4 bytes to 32 bit var*/
uint32_t read_8_to_32(int *i, uint8_t *src){
	uint32_t data = 0;
	data |= (src[(*i)++] << 0) ; 
	data |= (src[(*i)++] << 8) ; 
	data |= (src[(*i)++] << 16); 
	data |= (src[(*i)++] << 24); 
	return data;
}

/* Write 2 bytes to 16 bit var*/
uint16_t read_8_to_16(int *i, uint8_t *src){
	uint16_t data = 0;
	data |= (src[(*i)++] << 0) ;
	data |= (src[(*i)++] << 8) ;
	return data;
}
