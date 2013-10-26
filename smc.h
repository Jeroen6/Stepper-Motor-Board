/**
 * @file    SMC.h
 * @brief   Stepper motor control module
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @{
 */
#ifndef StepperMotorControl_H_
#define StepperMotorControl_H_

#include <stdint.h>

/**
 * @brief Motor control microstepping with Sine or Triangle
 * @{
 */
#define LUT_SINE			 	1
#define LUT_TRIANGLE 		0
#if (LUT_SINE == LUT_TRIANGLE)
	#error At least one LUT needs to Enabled or Disabled, not both.
#endif
 /**
  * @}
	*/

/* Api */
void SMC_init(void);
int SMC_step(int steps, uint8_t dir, uint32_t time_ms, uint8_t free);
uint32_t SMC_idle(void);
void SMC_deinit(void);
void SMC_brake(void);
void SMC_free(void);
void SMC_pause(void);
void SMC_continue(void);
int SMC_getState(void);
void SMC_egg(void);

/* Example of MOSFET bridges
 * ------------------------ V+
 * |			|			 |			|		
 * \H1		\H2		 \H3		\H4	
 * |--MA--|			 |--MB--|		
 * \L1		\L2		 \L3		\L4	
 * |			|			 |			|		
 * ------------------------ V- 
 */

/** 
 * @brief Motor control lookup tables 
 * @{
 */
const uint16_t LUT_H1[32] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const uint16_t LUT_H2[32] = {
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint16_t LUT_H3[32] = {
1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1
};

const uint16_t LUT_H4[32] = {
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
};   

#if (LUT_SINE == 1)  
	const uint16_t LUT_L1[32] = {
	0   	,102	,194 	,286 ,
	363 	,424	,471 	,501 ,
	512 	,501	,471 	,424 ,
	363 	,286	,194 	,102 ,
	0     ,0   	,0    ,0   ,
	0     ,0   	,0    ,0   ,
	0     ,0   	,0    ,0   ,
	0     ,0   	,0    ,0   
	};

	const uint16_t LUT_L2[32] = {
	0      ,0      ,0      ,0      ,
	0      ,0      ,0      ,0      ,
	0      ,0      ,0      ,0      ,
	0      ,0      ,0      ,0      ,
	0      ,102  ,194,286 ,
	363 ,424 ,471 ,501 ,
	512    ,501 ,471 ,424 ,
	363 ,286 ,194 ,102.  
	};

	const uint16_t LUT_L3[32] =  {
	0      ,0      ,0      ,0      ,
	0      ,0      ,0      ,0      ,
	0      ,102 ,194 ,286 ,
	363 ,424 ,471 ,501 ,
	512 ,501 ,471 ,424 ,
	363 ,286 ,194 ,102  ,
	0      ,0      ,0      ,0      ,
	0      ,0      ,0      ,0
	};

	const uint16_t LUT_L4[32] =  {
	512 ,501,471 ,424 ,
	363 ,286,194 ,102 ,
	0   ,0  ,0   ,0   ,
	0   ,0  ,0   ,0   ,
	0   ,0  ,0   ,0   ,
	0   ,0  ,0   ,0   ,
	0   ,102,194 ,286 ,
	363 ,424,471 ,501
	};
#endif // LUT_SINE
#if (LUT_TRIANGLE == 1)  
	const uint16_t LUT_L1[32] = {
	0  ,
	64 ,
	128,
	192,
	256,
	320,
	384,
	448,
	512,
	448,
	384,
	320,
	256,
	192,
	128,
	64 ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0   
	};

	const uint16_t LUT_L2[32] = {
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	64 ,
	128,
	192,
	256,
	320,
	384,
	448,
	512,
	448,
	384,
	320,
	256,
	192,
	128,
	64   
	};

	const uint16_t LUT_L3[32] =  {
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	64 ,
	128,
	192,
	256,
	320,
	384,
	448,
	512,
	448,
	384,
	320,
	256,
	192,
	128,
	64 ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  
	};

	const uint16_t LUT_L4[32] =  {
	512,
	448,
	384,
	320,
	256,
	192,
	128,
	64 ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	0  ,
	64 ,
	128,
	192,
	256,
	320,
	384,
	448
	};
#endif //LUT_TRIANGLE
 /**
  * @}
	*/
#endif //StepperMotorControl_H_
/**
 *@}
 */
