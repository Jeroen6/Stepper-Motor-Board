#include "mbed.h"
#include "CT32B0_PWM.h"

// Prescaler 48 gives 1 us per tick
#define CT32B0_PRESCALER 48	

/*  Since only one timer is used to supply 4 PWM channels 
		a problem occurs because CT32B0 only support 3 PWM channels.
	
		Since this software is designed for a H-Bridge implementation 
		only 2 of 4 PWM channels are required to be active simultaneously.
		
		This is where the stage variable comes in:
		Stage 0: PWM enabled on MAT 0, MAT 1 and MAT 2.
		Stage 1: PWM enabled on MAT 0, MAT 1 and MAT 3.
		Unused MAT outputs will be pullled down.
*/		
volatile uint8_t 	stage 	= 0;
volatile uint32_t period 	= 0;
volatile uint32_t mat[4];

void CT32B0_initpwm(uint32_t period_us, uint32_t defaultstate){
		// Calculte period
		period = period_us;

		// Store latest setting
		mat[0] = defaultstate;
		mat[1] = defaultstate;
		mat[2] = defaultstate;
		mat[3] = defaultstate;
		
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6)|(1<<9);

		LPC_CT32B0->IR		= 0x5F;							// Clear int 
		LPC_CT32B0->TCR		= 2;								// Reset
		LPC_CT32B0->PR		= CT32B0_PRESCALER; 
		LPC_CT32B0->CCR		= 0;	// No capture
		LPC_CT32B0->EMR		=	0;	// Not required, refer to PWMC
		LPC_CT32B0->CTCR	=	0;  // No special counters mode
		
		// Set outputs to stage
		if(!stage){
			// Stage 0, use MAT2 as period
			LPC_CT32B0->PWMC		=	(1<<0)|(1<<1)|(0<<2)|(1<<3);
			LPC_CT32B0->MCR			= (1<<7);		// Reset on MR2
			LPC_CT32B0->MR2 		= period;
			LPC_CT32B0->MR3 		= defaultstate;
			
			LPC_GPIO->DIR[1] 		|= (1<<26);
			LPC_GPIO->CLR[1] 		= (1<<26); 			// Note iocon invert
			LPC_IOCON->PIO1_26 	= 0x0 | (0x1<<3) ; 	// GPIO
			LPC_IOCON->PIO1_27 	= 0x1 | (0x1<<3) ;
		}else{
			// Stage 1, use MAT3 as period
			LPC_CT32B0->PWMC		=	(1<<0)|(1<<1)|(1<<2)|(0<<3);
			LPC_CT32B0->MCR			= (1<<10);	//Reset on MR3
			LPC_CT32B0->MR2 		= defaultstate;
			LPC_CT32B0->MR3 		= period;
			
			LPC_GPIO->DIR[1] 		|= (1<<27);			
			LPC_GPIO->CLR[1] 		= (1<<27);	
			LPC_IOCON->PIO1_26 	= 0x1 | (0x1<<3) ;	
			LPC_IOCON->PIO1_27	= 0x0 | (0x1<<3) ; // GPIO
		}
		
		// MAT 1 and MAT 2 are not affected by stage
		LPC_CT32B0->MR0 = defaultstate; 
		LPC_CT32B0->MR1 = defaultstate;
		LPC_IOCON->PIO1_24 = 0x1; // MAT and INVERT
    LPC_IOCON->PIO1_25 = 0x1;
		
		LPC_CT32B0->TCR		= 0;
}

void CT32B0_start(void){
	LPC_CT32B0->TCR		= 1;		// Enable
}

void CT32B0_deinit(uint8_t state){
  LPC_CT32B0->TCR		= 2;		// Disable and reset counter
	// Set all to GPIO
	LPC_IOCON->PIO1_24 = 0 | (0x1<<3);
  LPC_IOCON->PIO1_25 = 0 | (0x1<<3);
	LPC_IOCON->PIO1_26 = 0 | (0x1<<3);
	LPC_IOCON->PIO1_27 = 0 | (0x1<<3);
	LPC_GPIO->DIR[1] 	 |= (1<<24)|(1<<25)|(1<<26)|(1<<27);
	if(state)
		LPC_GPIO->SET[1] 	= (1<<24)|(1<<25)|(1<<26)|(1<<27);
	else
		LPC_GPIO->CLR[1] 	= (1<<24)|(1<<25)|(1<<26)|(1<<27);
}

void CT32B0_stage(uint8_t stagearg){
	stage = stagearg;
	// Set outputs to stage
	if(!stage){
		// Stage 0, use MAT2 as period
		LPC_CT32B0->PWMC		=	(1<<0)|(1<<1)|(0<<2)|(1<<3);
		LPC_CT32B0->MCR			= (1<<7);		// Reset on MR2
		LPC_CT32B0->MR2 		= period;
		LPC_CT32B0->MR3			=	mat[3];
		LPC_GPIO->CLR[1] 		= (1<<26);
		LPC_GPIO->SET[1]		= (1<<27);
		LPC_IOCON->PIO1_26 	= 0x0 | (0x1<<3); 	// GPIO
		LPC_IOCON->PIO1_27 	= 0x1 | (0x1<<3);
	}else{
		// Stage 1, use MAT3 as period
		LPC_CT32B0->PWMC		=	(1<<0)|(1<<1)|(1<<2)|(0<<3);
		LPC_CT32B0->MCR			= (1<<10);	//Reset on MR3
		LPC_CT32B0->MR2			=	mat[2];
		LPC_CT32B0->MR3 		= period;
		LPC_GPIO->CLR[1] 		= (1<<27);
		LPC_GPIO->SET[1]		= (1<<26);
		LPC_IOCON->PIO1_26	= 0x1 | (0x1<<3);
		LPC_IOCON->PIO1_27 	= 0x0 | (0x1<<3);	// GPIO
	}
}

void CT32B0_reload_mat(void){
	LPC_CT32B0->MR0 = mat[0]; 
	LPC_CT32B0->MR1 = mat[1];
	if(!stage){
   	LPC_CT32B0->MR3 = mat[3]; 
	}else{
	  LPC_CT32B0->MR2 = mat[2];
	}
}

void CT32B0_set(uint8_t matnr, uint32_t value){
	mat[matnr] = value;
	CT32B0_reload_mat();
}
