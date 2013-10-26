 /**
 * @file    CT16B1_PWM.cpp
 * @brief   Small driver to user CT16B1 for PWM
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @note		This only uses MAT 1
 * @{
 */
#include "mbed.h"
#include "CT16B1_PWM.h"

/* NOTICE: THIS MODULE ONLY WORKS FOR MAT 1 */

/* Static makes them private to this module */	
volatile static uint8_t  stage 	= 0;         /**< @brief Stage identifier for mat2/3 swap */
volatile static uint16_t period 	= 0;       /**< @brief PWM Period register */
volatile static uint16_t mat[4];             /**< @brief PWM Mat output registers */
volatile static uint16_t default_period_us;  /**< @brief Given period in us */
volatile static uint16_t defaultstate;       /**< @brief Default PWM state on init */
volatile static uint16_t prescaler;          /**< @brief Given prescaler */

/**
 * @brief   Initializes PWM
 *
 * @param[in] period_us   	Period in us, when prescaler is 48
 * @param[in] defaultstate  State after initializing
 * @param[in] prescaler			Divider from AHBCLK
 * @note Prescaler 48 gives 1 us timer ticks
 * @note Run Start() to start pwm
 */
void CT16B1_initpwm(uint16_t period_arg, uint16_t defaultstate, uint16_t prescaler_arg){
		// Calculte period
		period 		= period_arg;
		prescaler = prescaler_arg;

		// Store latest setting
		mat[0] = defaultstate;
		mat[1] = defaultstate;
		mat[2] = defaultstate;
		mat[3] = defaultstate;

		//  Enable timer clock, and GPIO
		LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6)|(1<<8);

		// Config timer
		LPC_CT16B1->IR		= 0x5F;							// Clear int 
		LPC_CT16B1->TCR		= 2;								// Reset
		LPC_CT16B1->PR		= prescaler; 
		LPC_CT16B1->CCR		= 0;	// No capture
		LPC_CT16B1->EMR		=	0;	// Not required, refer to PWMC
		LPC_CT16B1->CTCR	=	0;  // No special counters mode
		
		// Config mat 1
		LPC_CT16B1->PWMC		=	(0<<0)|(1<<1)|(0<<2)|(0<<3);	// MAT1 only
		LPC_CT16B1->MCR			= (1<<10);			// Reset on MR3
		LPC_CT16B1->MR3 		= period_arg;
		LPC_CT16B1->MR1 		= defaultstate;
		// Config met 1 IO
		LPC_IOCON->PIO0_22 	= 0x2 | (0x1<<3) ; 	// GPIO
		// Take timer out of reset
		LPC_CT16B1->TCR		= 0;
}

/**
 * @brief   Re-Initializes PWM
 */
void CT16B1_reinitpwm(void){
	CT16B1_initpwm(default_period_us, defaultstate, prescaler	);
}

/**
 * @brief   Start PWM
 */
void CT16B1_start(void){
	LPC_CT16B1->TCR		= 1;		// Enable
}

/**
 * @brief   Stop PWM
 * @param[in] state		PWM output state when pwm disabled
 */
void CT16B1_deinit(uint8_t state){
  LPC_CT16B1->TCR		= 2;		// Disable and reset counter
	// Set all to GPIO
	LPC_IOCON->PIO0_22 	= 0x0 | (0x1<<3) ; 	// GPIO
	LPC_GPIO->DIR[0] 	|= (1<<22);
	LPC_GPIO->CLR[0]	|= (1<<22);
}

/**
 * @brief  Reload all match compare registers
 */
void CT16B1_reload_mat(void){
	LPC_CT16B1->MR1 = mat[1]; 
	LPC_CT16B1->MR3 = period;
}

/**
 * @brief   Set channel PWM
 */
void CT16B1_set(uint8_t matnr, uint16_t value){
	mat[matnr] = value;
	CT16B1_reload_mat();
}

/**
 * @brief   Wait for timer to reach 0
 */
void CT16B1_wait_refresh(void){
	if( (LPC_CT16B1->TCR & 1) )
		while(LPC_CT16B1->TC != 0);
}

/**
 * @brief  Return period
 */
uint16_t CT16B1_get_period(void){
	return period;
}

/**
 * @brief   Return 1 if timer is started
 */
uint32_t CT16B1_isStarted(void){
	return LPC_CT16B1->TCR & 1;
}
/**
 *@}
 */
