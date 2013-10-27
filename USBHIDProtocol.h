#ifndef USBHIDPROTOCOL_H_
#define USBHIDPROTOCOL_H_
/* Command list */
// Devices verifies with same CMD and optional data

// System commands
	#define CMD_SYS_CHECK		0x00	// Args: version
	#define CMD_SYS_RESET		0xFF	// Args: bool softreset

// Miscellaneous
	#define CMD_LED_OFF			0x10	// On board led on
	#define CMD_LED_ON			0x11	// On board led off
	#define CMD_LED_BLINK		0x12	// On board led blink
	#define CMD_LED_BREATH		0x13	// Not implemented
	#define CMD_GET_BUTTON		0x14	// Returns button history word with 20 ms interval per bit

// SMC commands
	/* Motor has 3 states :
		Locked	-	Current flowing, motor is powered
		Brake	-	Motor terminals short circuit
		Free	-	Motor terminals open
	*/
	#define CMD_SMC_STEP		0x20	// Args, int number of steps, bool direction, finish this in .. (ms), bool free motor (or keep locked)
	#define CMD_SMC_STATE		0x21	// Returns remaining steps, negative if paused

	#define CMD_SMC_STOP		0x22	// Aborts all motor control
	#define CMD_SMC_PAUSE		0x23	// Pauzes motor control, keeps locked (overheating!)
	#define CMD_SMC_CONTINUE	0x24	// Continues motor control

	// Motor States (ineffective if stepping)
	#define CMD_SMC_FREE		0x25	// Free motor
	#define CMD_SMC_BRAKE		0x26	// Brake motor (different from lock)

// AUX channel
	#define CMD_AUX_OFF			0x30	// Auxilary channel ON	(discards pwm)
	#define CMD_AUX_ON			0x31	// Auxilary channel OFF (discards pwm)
	#define CMD_AUX_PWM			0x32	// Auxilary channel PWM. Args: duty cycle, period, timer prescaler (from 48 MHz)
	
#endif //USBHIDPROTOCOL_H_
