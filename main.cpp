#include "mbed.h"
#include "SMC.h"
#include "watchdog.h"
#include "USBHID.h"

#pragma anon_unions
/* Main routine handles USB and SMC api */

// USB HID Objects
USBHID *hid;
HID_REPORT send_report;
HID_REPORT recv_report;

// Only global memory allocation
DigitalOut led(P0_21);
InterruptIn button(P0_1);

void button_handler(void){
	wait(0.1);
	WatchDog_us bello(100);
	// Reset by watchdog does not iniate bootloader when button is low
}

int main() {
	LPC_SYSCON->SYSRSTSTAT = 0x1F;
  button.fall(&button_handler);
	// Motor Control IO config
	SMC_init();
	// LED
	
	// USB Initialize	
	static USBHID hid_object(12, 12);
	hid = &hid_object;
	send_report.length = 12;
	
	led = 0;
	// Motor selftest
	//SMC_step(1600, 1, 1200000, 1);
	//while( !SMC_idle() );
	//SMC_step(1600, 1, 10000, 1);
	//while( !SMC_idle() );
	
	typedef  struct  {
		uint8_t cmd;
		uint8_t data[11];
	} USB_command_t; 
	

	static step_command_t scmd;
	static USB_command_t cmd;
	// Ready
  while(1){
		// Wait for command
		while( !hid->readNB(&recv_report) );
		led = !led;
		// Decode command
	  //cmd = (USB_command_t*)&recv_report.data;
		//uint32_t *d = (uint32_t*)&cmd, *s = (uint32_t*)&recv_report.data[1];
		
		/*for(int i=0; i<3; i++){
				*d++ = *s++;
		} */
		
		cmd.cmd = recv_report.data[0];
		scmd.steps = 0;
		scmd.steps |=  (recv_report.data[1]<<24)    ;
		scmd.steps |=  (recv_report.data[2]<<16)    ;
		scmd.steps |=  (recv_report.data[3]<<8)     ;
		scmd.steps |=  (recv_report.data[4])        ;
		scmd.dir |= recv_report.data[5]                 ;
		scmd.time_ms = 0;
		scmd.time_ms |= (recv_report.data[6]<<24)  ;
		scmd.time_ms |= (recv_report.data[7]<<16)  ;
		scmd.time_ms |= (recv_report.data[8]<<8)   ;
		scmd.time_ms |= (recv_report.data[9])      ;
		scmd.free = recv_report.data[10]                ;
		//recv_report.data[11];
		
		switch(cmd.cmd){
		case 1:	// Step Command
				if(0 == SMC_step(scmd.steps, scmd.dir, scmd.time_ms, scmd.free )){
					while( !SMC_idle() );
					// return ok
					send_report.data[0] = 1;
					send_report.data[1] = 0;
					hid->send(&send_report);
					send_report.data[0] = 0;
					send_report.data[1] = 0;
				}else{
					// argument failure
					send_report.data[0] = 1;
					send_report.data[1] = 0xFF;
					hid->send(&send_report);
					send_report.data[0] = 0;
					send_report.data[1] = 0;
				}
				break;
		case 2:	// Stop step command
			SMC_deinit();
		case 0:
		default:
			send_report.data[0] = 0xFF;
			send_report.data[1] = cmd.cmd;
			send_report.data[2] = 0;
			send_report.data[3] = 0;
			send_report.data[4] = 0;
			hid->send(&send_report);
			send_report.data[0] = 0;
			send_report.data[1] = 0;
			send_report.data[2] = 0;
			send_report.data[3] = 0;
			send_report.data[4] = 0;
			break;
		}	
	}
}

