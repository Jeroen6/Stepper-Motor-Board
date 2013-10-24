#ifndef StepperMotorControl_H_
#define StepperMotorControl_H_

#include <stdint.h>

/* Api */
void SMC_init(void);
//void SMC_routine(void);
int SMC_step(int steps, uint8_t dir, uint32_t time_ms, uint8_t free);
uint32_t SMC_idle(void);
void SMC_deinit(void);

void SMC_brake(void);
void SMC_free(void);

void SMC_pause(void);
void SMC_continue(void);

int SMC_getState(void);

/* Lookup tables */
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

#endif //StepperMotorControl_H_
