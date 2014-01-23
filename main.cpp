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
#include "xifo_sizetype.h"

/* Main routine handles USB and SMC api */

#define VERSION	0x03

/**
 * @brief   Serial declarations
 * @{
 */
#define UART_REPORT_SIZE	16
Serial uart(P1_13, P1_14); // tx, rx
volatile uint8_t serialPacketReady = 0;
xifo_t uartbuf;  // Used as FIFO
xifo_pool_t uartbufpool[64] __attribute__((aligned (4)));
//volatile uint32_t Original_UART_IRQn_Address;
//volatile uint32_t uartBreak = 0;

Ticker Ticker_BufferAging;
#define Ticker_BufferAging_interval	5000				// micro seconds uS
#define Ticker_BufferAging_timeout	200 * 5000	// 1 Seconds
volatile uint32_t bufferAge = 0;

/** 
 * @} 
 */
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
volatile uint32_t button_history = 0xFFFFFFFF;
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



/**
 * @brief   Serial Input Handler
 * @note    Buffers a packet before clearing it for processing
 */
void serialHandler(void){
	/*
	if(uartBreak){
		uartBreak = 0;
		// reset buffer
		xifo_init(&uartbuf,sizeof(uartbufpool),uartbufpool);
		xifo_clear(&uartbuf);
		serialPacketReady = 0;
	}else{
	*/
		uint32_t wdog = 100;
		while(uart.readable() && wdog != 0 ){
			xifo_write(&uartbuf, uart.getc());  
		}
		if( xifo_get_used(&uartbuf) >= UART_REPORT_SIZE ){
				// Assume full command packet
				serialPacketReady++;
		}
		bufferAge = 0;
	//}
}

/**
 * @brief   Buffer Aging Handler
 * @note    Clears the input buffer when the newest byte is onder than 1 second
 */
void serialBufferAgingHandler(void){
	if(bufferAge++ > Ticker_BufferAging_timeout){
		xifo_init(&uartbuf,sizeof(uartbufpool),uartbufpool);
		xifo_clear(&uartbuf);
		serialPacketReady = 0;
	}
}


/**
* @brief   Remapped UART interrupt
* @note    Since the mbed library and intterupts are precompiled
*/
/*
void  UART_IRQ_Custom(void){
	// Do our stuff
	if( LPC_USART->LSR & (1<<4) )
		uartBreak++;
	((void (*)(void))Original_UART_IRQn_Address)();	
	
} */  
 
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
	// UART initialize
	uart.format(8, Serial::None, 1);
	uart.baud(115200);
	uart.attach(serialHandler);
	xifo_init(&uartbuf,sizeof(uartbufpool),uartbufpool);
	xifo_clear(&uartbuf);
	Ticker_BufferAging.attach_us(&serialBufferAgingHandler, Ticker_BufferAging_interval);
	// remap uart interrupt so we can add stuff
	/*
	Original_UART_IRQn_Address = NVIC_GetVector(UART_IRQn);
	NVIC_SetVector(UART_IRQn, (uint32_t)&UART_IRQ_Custom);
	LPC_USART->IER |= (1<<2); // enable line status interrupt
	*/
	// button config
	Ticker_button.attach_us(&Ticker_button_handler, 20000);	
	
	// Ready
	while(1){
		uint8_t UART_PacketParsed = 0;
		// insert idle time, since we cant be running USB stuff all the time
		wait_us(5000);
		
		if(button_history < 0xFFFFFFFF){
			SMC_step(1600, 1, 30000, 1);
			while(!SMC_idle());
			continue;
		}
			
		
	/* First read from UART, since UART uses the same command structures as USB
	 * Commands from UART are loaded into USB recv_report	to reduce code size 
	 * and decrease required UART development time 
	 */
		
	// try to read a msg form UART	
		if( serialPacketReady ){
			serialPacketReady--;
			// Load UART buffer into recv_report (FIFO MODE)
			{
				empty_report(&recv_report);
				recv_report.length = 64;
				uint8_t *p = recv_report.data;
				while( xifo_get_used(&uartbuf) ){
						*p++ = xifo_pop_lr(&uartbuf);
				}
				xifo_init(&uartbuf,sizeof(uartbufpool),uartbufpool);
				xifo_clear(&uartbuf);
			}
			// Data packet received, start parsing
			int irx=0;
			int itx=0;
			send_report.data[itx++] = recv_report.data[0];
			// Include large switch case module
			{
				#include "USB_SwitchCase.c"
			}
			// Return command + optional new args
			{
				uint8_t *p = send_report.data;
				uint32_t i;
				send_report.length = 32;
				i = send_report.length;
				while(i--) while(uart.writeable()) uart.putc(*p);
			}
			// 0xFF unused bytes
			empty_report(&recv_report);			
			empty_report(&send_report);
			// Set flag to skip USB packet parsing
			UART_PacketParsed++;
		}	// if uart readable
		
	//try to read a msg from USB
		if( hid->readNB(&recv_report) && !UART_PacketParsed ){
			// Data packet received, start parsing
			int irx=0;
			int itx=0;
			send_report.data[itx++] = recv_report.data[0];
			// Include large switch case module
			{
				#include "USB_SwitchCase.c"
			}
			// Return command + optional new args
			hid->send(&send_report);
			// 0xFF unused bytes
			empty_report(&recv_report);			
			empty_report(&send_report);
		}	// if packet
		
	} // While
} // Main
/** @} */
