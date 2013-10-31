 /**
 * @file    main.c
 * @brief   Main thread controlling everything
 * @details This main thread received USB commands and executes them to other modules					
 *
 * @Author	Jeroen Lodder
 * @Date		October 2013
 *
 * @{
 */
 
#include "mbed.h"
#include "smc.h"
#include "watchdog.h"
#include "USBHID.h"
#include "ByteOperations.h"
#include "USBHIDProtocol.h"
#include "CT16B1_PWM.h"

/* Main routine handles USB and SMC api */

#define VERSION	3

/**
 * @brief   USB HID Declarations
 * @{
 */
USBHID *hid;
HID_REPORT send_report __attribute__((aligned (4))); // Aligned for fast access
HID_REPORT recv_report __attribute__((aligned (4))); // Aligned for fast access
/** 
 * @} 
 */

/* Input Output */
DigitalOut led(P0_21);  /**< @brief User led */
DigitalIn button(P0_1); /**< @brief User button input (bootloader pin) */
DigitalOut aux(P0_22); 	/**< @brief Auxilary mosfet channel */

/**
 * @brief		Debounced button reader
 */
Ticker Ticker_button;
volatile uint32_t button_history = 0xFF;
void Ticker_button_handler(void){
	button_history <<= 1;
	button_history |= button;
}

/**
 * @brief   LED Blink Handler
 */
Ticker Ticker_led;
void Ticker_led_handler(void){
	led = !led;
}

/**
 * @brief   Clear buffer contents
 * @note    Requires 4 byte alignement of data
 * @param[in] c   Pointer to @p HID_REPORT.
 */
void empty_report(HID_REPORT *data){
	register uint32_t *p = (uint32_t *)data->data;
	for( register int i=0; i<((sizeof(HID_REPORT)-1)/4); i++ ){
		*p = 0xFFFFFFFF;
		p++;
	}
}
 
/* Program entry */
int main(void){
/* Init */
	// reset flags
	LPC_SYSCON->SYSRSTSTAT = 0x1F;
	// Motor Control IO config
	SMC_init();
	// aux init
	aux = 0;
	// LED
	led = 0;
	// USB Initialize	
	static USBHID hid_object(64, 64);
	hid = &hid_object;
	send_report.length = 64;
	// button config
	Ticker_button.attach_us(&Ticker_button_handler, 20000);
		
	// Ready
	while(1){
		// insert idle time, since we cant be running USB stuff all the time
		wait_us(5000);		
		//try to read a msg
		if(hid->readNB(&recv_report)){
			// Data packet received, start parsing
			int irx=0;
			int itx=0;
			send_report.data[itx++] = recv_report.data[0];
			switch( recv_report.data[irx++] ){
				case 	CMD_SYS_CHECK		:
					send_report.data[itx++]	=  VERSION;
				break;
				case 	CMD_SYS_RESET		: 
					  // Soft reset
						SMC_deinit();
						SMC_init();
						CT16B1_deinit(0);
						led = 1;
						Ticker_led.detach();
						aux = 0;
						empty_report(&recv_report);
				break;
				case 	CMD_LED_OFF			:
					led = 1;	// inverted led
					Ticker_led.detach();
				break;
				case 	CMD_LED_ON			:
					led = 0;	// inverted led
					Ticker_led.detach();
				break;
				case 	CMD_LED_BLINK		:
					Ticker_led.attach_us(&Ticker_led_handler, 100000);
				break;
				case 	CMD_LED_BREATH	:
					// Not implemented
				break;
				case 	CMD_GET_BUTTON	:
					write_32_to_8(&itx, send_report.data, button_history);					
				break;
				case 	CMD_SMC_STEP		:	{		
					uint32_t arg_step =	read_8_to_32(&irx, recv_report.data);				
					uint8_t  arg_dir 	=	recv_report.data[irx++];
					uint32_t arg_time =	read_8_to_32(&irx, recv_report.data);				
					uint8_t  arg_free	=	recv_report.data[irx++];
					SMC_step(arg_step, arg_dir, arg_time, arg_free);
				}
				break;
				case 	CMD_SMC_STATE		:
					write_32_to_8(&itx, send_report.data, SMC_getState());	
				break;
				case 	CMD_SMC_STOP		:
					SMC_deinit();
				break;
				case 	CMD_SMC_PAUSE		:
					SMC_pause();
				break;
				case 	CMD_SMC_CONTINUE:
					SMC_continue();
				break;
				case 	CMD_SMC_FREE		:
					SMC_free();
				break;
				case 	CMD_SMC_BRAKE		:
					SMC_brake();
				break;
				case 	CMD_AUX_OFF			:
					CT16B1_deinit(0);
					aux = 0;	
					//if(CT16B1_isStarted())
					
				break;
				case 	CMD_AUX_ON			:
					CT16B1_deinit(1);
					aux = 1;
					//if(CT16B1_isStarted())
				break;
				case 	CMD_AUX_PWM			: {
					uint16_t dutycycle, period, prescaler;
					dutycycle = read_8_to_16(&irx, recv_report.data);
					period 		= read_8_to_16(&irx, recv_report.data);
					prescaler = read_8_to_16(&irx, recv_report.data);
					/* Invert PWM, since timer is inverted */
					dutycycle = period - dutycycle;
					if(CT16B1_isStarted()){
					  CT16B1_wait_refresh();
						CT16B1_set(1, dutycycle);
					}else{
						CT16B1_initpwm(period,dutycycle,prescaler);
						CT16B1_start();
					}
				}					
				break;
				case 0xEE	: {
					hid->sendNB(&send_report);	
					SMC_egg();
					WatchDog_us bello(100);
					}
				break;
				default:
					send_report.data[0] =	0xFF;	//Failure
				break;
			} // Switch	
			// Return command + optional new args
			hid->send(&send_report);
			// 0xFF unused bytes
			empty_report(&recv_report);			
			empty_report(&send_report);
		}		// if packet
	} // While
} // Main
/** @} */
