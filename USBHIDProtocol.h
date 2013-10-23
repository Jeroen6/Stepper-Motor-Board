#ifndef USBHIDPROTOCOL_H_
#define USBHIDPROTOCOL_H_
/* Command list */
// Devices verifies with same CMD and optional data

// System commands
	#define CMD_SYS_CHECK		0x00	// Args: version
	#define CMD_SYS_RESET		0xFF

// Miscellaneous
	#define CMD_LED_OFF			0x10
	#define CMD_LED_ON			0x11
	#define CMD_LED_BLINK		0x12	// Dummy
	#define CMD_LED_BREATH		0x13	// Dummy
	#define CMD_GET_BUTTON		0x14	// Returns button events in last ... time

// SMC commands
	#define CMD_SMC_STEP		0x20	// Args
	#define CMD_SMC_STATE		0x21	// Args, device sends this every step with remaining steps

	#define CMD_SMC_STOP		0x22
	#define CMD_SMC_PAUSE		0x23
	#define CMD_SMC_CONTINUE	0x24

	// Motor States (ineffective if stepping)
	#define CMD_SMC_FREE		0x25
	#define CMD_SMC_BRAKE		0x26

// AUX channel
	#define CMD_AUX_OFF			0x30	
	#define CMD_AUX_ON			0x31
	#define CMD_AUX_PWM			0x32	// Args:
	
#endif //USBHIDPROTOCOL_H_
