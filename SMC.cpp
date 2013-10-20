#include "mbed.h"
#include "smc.h"
#include "CT32B0_PWM.h"

#define PWM_PERIOD				513
#define MIN_STEPTIME_US  	100

/* Motor Control IO */
DigitalOut HIFET_1(P0_16);
DigitalOut HIFET_2(P0_17);
DigitalOut HIFET_3(P0_18);
DigitalOut HIFET_4(P0_19);

/* Motor Control stepcontrol */
Ticker smc;
volatile int smc_walker	=	0;
volatile int smc_dir 		= 1;
volatile int smc_steps	= -1;
volatile int smc_free		= 1;
volatile int smc_abort	= 0;

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
	
	// Phase 1 A	
	// If sin +, H1->L2
	// If sin -, H2->L1
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
		smc_abort = 1;
	}
	
	#undef i
	
	/* Walk */
	smc_walker += smc_dir;
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
	}
	smc_steps--;
	}
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
	if(dir == 0) smc_dir = -1; else smc_dir = 1;
	// Configure stepper
	smc_steps = steps;
	smc_free	= free;
	// Initiate
	SMC_init();				
	smc.attach_us(&SMC_routine, steptime);
	return 0;	
}

int SMC_step_cmd(step_command_t *scmd){
	return -1;
}

uint32_t SMC_idle(void){
	if(smc_steps == -1)
		return 1;
	else
		return 0;
}


