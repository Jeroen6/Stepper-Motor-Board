 
 /**
 * @file    xifo.h
 * @brief   xifo circular buffer
 * @details xifo supplies object oriented circular buffer with 32 bit size elements. \n
 * 					To use either as FIFO (First In First Out) or as FILO (First In Last Out)
 *
 * @author	Jeroen Lodder
 * @date		April 2013
 * @version 2
 *
 * @{
 */


#ifndef _xifo_H_
#define _xifo_H_

#include <inttypes.h>
#define SIZETYPE uint8_t

#if defined(__arm__)  || defined(__DOXYGEN__)
#pragma anon_unions				/**< Allow unnamed unions */
#endif

/**
 * @brief   Circular Buffer object.
 * @details This struct holds the object of a circular buffer
 */
typedef struct  {
/* Pointers: */
		SIZETYPE *startpool;  /**< @brief First element in pool */
		SIZETYPE *endpool;		/**< @brief Last element in pool */
		SIZETYPE *read;				/**< @brief Read pointer */
		SIZETYPE *write;			/**< @brief Write pointer */
/* Variables: */
		SIZETYPE full;        /**< @brief Flag indicating buffer is full */
    uint32_t elementcount;/**< @brief Number of elements used */
		uint32_t size;				/**< @brief Size of buffer */
/* Locally used in functions to prevent stack use: */
		/**< @brief union to prevent lvalue typecasting */  
		union {					     
			SIZETYPE temp;			/**< @brief temp variable and padding for even sized block */
			SIZETYPE *ptemp; 		/**< @brief temp variable and padding for even sized block */
		};
}xifo_t;

/**
 * @brief   Circular Buffer memory pool type.
 */
typedef SIZETYPE xifo_pool_t;

#ifdef __cplusplus
extern "C" {
#endif
/* xifo Common */
void xifo_init(xifo_t *c, uint32_t size, xifo_pool_t *startpool);
void xifo_clear(xifo_t *c);
uint32_t xifo_write(xifo_t *c, SIZETYPE data);
/* FIFO use */
SIZETYPE xifo_read_lr(xifo_t *c, uint32_t index);
SIZETYPE xifo_pop_lr(xifo_t *c);
/* LIFO use */
SIZETYPE xifo_read_mr(xifo_t *c, uint32_t index);
SIZETYPE xifo_pop_mr(xifo_t *c);
/* Extractors */
uint32_t xifo_get_size(xifo_t *c);
uint32_t xifo_get_used(xifo_t *c);
uint32_t xifo_get_full(xifo_t *c);
uint32_t xifo_get_free(xifo_t *c);
#ifdef __cplusplus
}
#endif
#endif //_xifo_H_

/** @} */
