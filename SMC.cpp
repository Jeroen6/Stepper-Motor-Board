#include "mbed.h"
#include "smc.h"
#include "CT32B0_PWM.h"
#include "CT16B1_PWM.h"

#define PWM_PERIOD				513
#define MIN_STEPTIME_US  	100

/* Motor Control IO */
DigitalOut HIFET_1(P0_16);
DigitalOut HIFET_2(P0_17);
DigitalOut HIFET_3(P0_18);
DigitalOut HIFET_4(P0_19);

/* Motor Control stepcontrol */
Ticker smc;
volatile static int smc_walker		=	0;
volatile static int smc_dir 			= 1;
volatile static int smc_steps			= -1;
volatile static int smc_free			= 1;
volatile static int smc_abort			= 0;
volatile static int smc_steptime 	= 0;
volatile static int smc_isPaused 	= 0;

void SMC_init(void){
	// Hi fet low
  HIFET_1 = 0;
	HIFET_2 = 0;
	HIFET_3 = 0;
	HIFET_4 = 0;
	// Wait minimum
	
	// Lo fet low
  CT32B0_initpwm(PWM_PERIOD,0);
	// Start pwm
	CT32B0_start();
}

void SMC_deinit(void){
	smc.detach();
	CT32B0_deinit(0);
	
  HIFET_1 = 0;
	HIFET_2 = 0;
	HIFET_3 = 0;
	HIFET_4 = 0;

	smc_walker	=	0;
	smc_dir 		= 1;
	smc_steps	= -1;
	smc_free		= 1;
  smc_abort	= 0;
}

void SMC_routine(void){
	#define i	smc_walker
	CT32B0_wait_refresh();
	__disable_irq();
	// Phase 1 A	
	// If sin +, H1->L2
	// If sin -, H2->L1
	// If direction -1, swap motor A channels, this changes direction.
	// Reversing lookup table is not effective
	
	if(smc_dir > 0){
		HIFET_1 = LUT_H1[i];
		HIFET_2 = LUT_H2[i];
		if(LUT_L1[i] == 0)
			CT32B0_set(0,PWM_PERIOD+1);
		else
			CT32B0_set(0,PWM_PERIOD-LUT_L1[i]);
		if(LUT_L2[i] == 0)
			CT32B0_set(1,PWM_PERIOD+1);
		else
			CT32B0_set(1,PWM_PERIOD-LUT_L2[i]);
	}else{
		// Reversed for dir -1
		HIFET_1 = LUT_H2[i];
		HIFET_2 = LUT_H1[i];
		if(LUT_L2[i] == 0)
			CT32B0_set(0,PWM_PERIOD+1);
		else
			CT32B0_set(0,PWM_PERIOD-LUT_L2[i]);
		if(LUT_L1[i] == 0)
			CT32B0_set(1,PWM_PERIOD+1);
		else
			CT32B0_set(1,PWM_PERIOD-LUT_L1[i]);
	}

	// Phase 1 A	
	// If sin +, H1->L2
	// If sin -, H2->L1
	HIFET_3 = LUT_H3[i]; 
	HIFET_4 = LUT_H4[i]; 
	if(LUT_L3[i] == 0)
		CT32B0_set(2,PWM_PERIOD+1);
	else
		CT32B0_set(2,PWM_PERIOD-LUT_L3[i]);
	if(LUT_L4[i] == 0)
		CT32B0_set(3,PWM_PERIOD+1);
	else
		CT32B0_set(3,PWM_PERIOD-LUT_L4[i]);
	if(i==9)
		CT32B0_stage(1);
	if(i==24)
		CT32B0_stage(0);
	CT32B0_reload_mat();	
	
	/*
	volatile static uint8_t hifet[4] = {0,0,0,0};
	hifet[0] = HIFET_1;
	hifet[1] = HIFET_2;
	hifet[2] = HIFET_3;
	hifet[3] = HIFET_4;
	volatile static uint8_t lofet[4] = {0,0,0,0};
	if(LUT_L1[i])	lofet[0] = 1; else lofet[0] = 0;
	if(LUT_L2[i])	lofet[1] = 1; else lofet[1] = 0;
	if(LUT_L3[i])	lofet[2] = 1; else lofet[2] = 0;
	if(LUT_L4[i])	lofet[3] = 1; else lofet[3] = 0;
	volatile static uint8_t errfet[4] = {0,0,0,0};
	errfet[0] = hifet[0] & lofet[0];
	errfet[1] = hifet[1] & lofet[1];
	errfet[2] = hifet[2] & lofet[2];
	errfet[3] = hifet[3] & lofet[3];
	
	if( errfet[0] | errfet[1] | errfet[2] | errfet[3] ){
		// ILLEGAL MODE
	//	smc_abort = 1;
		 __NOP();
	} */
	
	#undef i
	
	/* Walk */
	smc_walker += abs(smc_dir);
	if(smc_walker > 31)
	smc_walker = 0;
	if(smc_walker < 0)
	smc_walker = 31;
	/* Coutdown */
	if(smc_steps != -1){
		if(smc_steps == 0 || smc_abort == 1){
			if(smc_free || smc_abort == 1){
				// motor free
				HIFET_1 = 0;
				HIFET_2 = 0;
				HIFET_3 = 0;
				HIFET_4 = 0;
				CT32B0_deinit(0);
			}else{
				// motor locked
			}
			smc.detach();
			smc_abort = 0;
			smc_isPaused = 0;
		}
	smc_steps--;
	}
	__enable_irq();
}

int SMC_step(int steps, uint8_t dir, uint32_t time_ms, uint8_t free){
	// steps   = number of microsteps (8 microsteps per full step)
	// dir	   = -1 or 1
	// time_us = completion time in s
	uint32_t steptime = (time_ms*1000)/steps;	
	// Parameter check
	if(smc_steps != -1)	return -1;	// Only if motor idle
	if(steps < 1) return -1;        // At least one step
	if(dir) dir = 1;            		// Round dir to bool
	if(free) free = 1;        			// Round free to bool
	if(steptime < MIN_STEPTIME_US)  // Verify steptime
			return -1;
	if(dir == 0) 
		smc_dir = -1; 
	else 
		smc_dir = 1;
	// Configure stepper
	smc_steps = steps;
	smc_free	= free;
	smc_steptime = steptime;
	smc_isPaused = 0;
	// Initiate
	SMC_init();				
	smc.attach_us(&SMC_routine, smc_steptime);
	return 0;	
}

uint32_t SMC_idle(void){
	if(smc_steps == -1)
		return 1;
	else
		return 0;
}

void SMC_brake(void){
	// Do not brake when active control
	if(smc_walker > 0)
		return;
	// Regular deinit
	SMC_deinit();
	// Re-enable low side
	CT32B0_deinit(1);
}

void SMC_free(void){
	// Do not free when active control
	if(smc_walker > 0)
		return;
	// Regular deinit
	SMC_deinit();
	// Re-enable low side
	CT32B0_deinit(0);
}

void SMC_pause(void){
	smc.detach();
	smc_isPaused = 1;
}

void SMC_continue(void){
	smc.attach_us(&SMC_routine, smc_steptime);
	smc_isPaused = 0;
}

int SMC_getState(void){
	if( smc_steps < 0 ){
		return 0;
	}else{
	  if(smc_isPaused)
			return (-1*smc_steps);
		else
		  return (1*smc_steps);
	}
}
void SMC_egg(void){
const	uint16_t rr[] = {
		627  ,		1045 ,
		122  , 		785  ,
		887  , 		187  ,
		233  ,		166  ,
		788  , 		752  ,
		1157

	};
	int rri=0;
	// Egg
	for(int i=0;i<2; i++){
		SMC_step(rr[rri++]	, 1, 685, 1);		// 83 - 987
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 965, 1);	// 85 - 1108
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 245, 1);   // 78 - 739
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 725, 1);   // 85 - 1108
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 710, 1);   // 87 - 1244
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 125, 1);   // 90 - 1479
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 175, 1);   // 88 - 1318
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 133, 1);   // 87 - 1244
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 860, 1);   // 83 - 987
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 695, 1);   // 85 - 1108
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		SMC_step(rr[rri++]	, 1, 2315, 1); // 78 -  739
		while( !SMC_idle()) ;       //    -
		wait_ms(1);
		rri=0;
	}
}
