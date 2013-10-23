#include "mbed.h"
#include "SMC.h"
#include "watchdog.h"
#include "USBHID.h"
#include "ByteOperations.h"
#include "USBHIDProtocol.h"
#include "WatchDog.h"

#pragma anon_unions
/* Main routine handles USB and SMC api */

#define VERSION	0x00

// USB HID Objects
USBHID *hid;
HID_REPORT send_report __attribute__((aligned (4))); // Aligned for fast access
HID_REPORT recv_report __attribute__((aligned (4))); // Aligned for fast access

/* Input Output */
DigitalOut led(P0_21);
DigitalIn button(P0_1);
DigitalOut aux(P0_22);

/* Tickers */
/* Debounced button handler */
Ticker Ticker_button;
volatile uint32_t button_history = 0xFF;
void Ticker_button_handler(void){
	button_history <<= 1;
	button_history |= button;
}
/* Led blink handler */
Ticker Ticker_led;
void Ticker_led_handler(void){
	led = !led;
}

/* Functions */
/* Fast erase usb report */
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

	
	// Motor selftest
	//SMC_step(1600, 1,15000, 1);
	//while( !SMC_idle() );
	
	// Ready
	while(1){
		//try to read a msg
		if(hid->readNB(&recv_report)){
			// Data packet received, start parsing
			empty_report(&send_report);
			int irx=0;
			int itx=0;
			send_report.data[itx++] = recv_report.data[0];
			switch( recv_report.data[irx++] ){
				case 	CMD_SYS_CHECK		:
					send_report.data[itx++]	=  VERSION;
				break;
				case 	CMD_SYS_RESET		: 
					if(recv_report.data[irx++]){
						// Hard reset, causes disconnect!
						hid->send(&send_report);
						WatchDog_us bello(100);
					}else{
					  // Soft reset
						#warning deinit smc
						led = 1;
					}
				break;
				case 	CMD_LED_OFF			:
					led = 1;	// inverted led
					Ticker_led.detach();
				break;
				case 	CMD_LED_ON			:
					led = 0;	// inverted led
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
					write_32_to_8(&itx, send_report.data, 0x12345678);	
				break;
				case 	CMD_SMC_STOP		:
					#warning SMC_Stop implement here
				break;
				case 	CMD_SMC_PAUSE		:
					#warning SMC_ticker detach
				break;
				case 	CMD_SMC_CONTINUE:
					#warning SMC_ticker attach if not -1
				break;
				case 	CMD_SMC_FREE		:
					#warning stop+de_init motor when no running command, free
				break;
				case 	CMD_SMC_BRAKE		:
					#warning stop+de_init motor when no running command, brake
				break;
				case 	CMD_AUX_OFF			:
					aux = 0;	
				break;
				case 	CMD_AUX_ON			:
					aux = 1;
				break;
				case 	CMD_AUX_PWM			:
					/* Not implemented */
				break;
				default:
					send_report.data[0] =	0xFF;	//Failure
				break;
			} // Switch	
			// Return command + optional new args
			hid->send(&send_report);	
		} // if packet
	}
}

