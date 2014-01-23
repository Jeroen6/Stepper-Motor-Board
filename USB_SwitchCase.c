/*
	USB_SwitchCase.c IS INCLUDED IN main.cpp FROM int main(void), 
	DO NOT COMPILE SEPARATE
*/			
			
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
