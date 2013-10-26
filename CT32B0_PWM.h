 /**
 * @file    CT32B0_PWM.h
 * @brief   Small driver to user CT32B0 for PWM
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @note		Since only one timer is used to supply 4 PWM channels 
 * 					a problem occurs because CT32B0 only support 3 PWM channels.
 * 					
 * 					Since this software is designed for a H-Bridge implementation 
 * 					only 2 of 4 PWM channels are required to be active simultaneously.
 * 					
 * 					This is where the stage variable comes in:
 * 					Stage 0: PWM enabled on MAT 0, MAT 1 and MAT 2.
 * 					Stage 1: PWM enabled on MAT 0, MAT 1 and MAT 3.
 * 					Unused MAT outputs will be pullled down.
 * @{
 */
#ifndef CT32B0_PWM_H_
#define CT32B0_PWM_H_

void CT32B0_initpwm(uint32_t period, uint32_t defaultstate);
void CT32B0_reinitpwm();
void CT32B0_start(void);
void CT32B0_stage(uint8_t stagearg);
void CT32B0_reload_mat(void);
void CT32B0_set(uint8_t matnr, uint32_t value);
void CT32B0_deinit(uint8_t state);
void CT32B0_wait_refresh(void);

#endif //CT32B0_PWM_H_
/**
 *@}
 */
