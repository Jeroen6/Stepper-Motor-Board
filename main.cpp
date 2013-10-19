#include "mbed.h"
#include "SMC.h"
#include "USB.h"
#include "watchdog.h"

#include "CT32B0_PWM.h"
/* Main routine handles USB and SMC api */

// Only global memory allocation
DigitalOut led(P0_21);
InterruptIn button(P0_1);

void button_handler(void){
	WatchDog_us bello(100);
	
}

int main() {
	LPC_SYSCON->SYSRSTSTAT = 0x1F;
  button.fall(&button_handler);
	// Motor Control IO config
	SMC_init();
	// LED
	DigitalOut led(P0_21);
	// USB Initialize	
	USB_initialize();
	
  while(1){
		USB_routine();
		
		SMC_step(1600, 1, 10000, 1);
		
		wait_ms(500);
		
		led = !led;
	}
}

