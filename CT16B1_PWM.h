 /**
 * @file    CT16B1_PWM.h
 * @brief   Small driver to user CT16B1 for PWM
 *
 * @author	Jeroen Lodder
 * @date		Oktober 2013
 *
 * @note		This only uses MAT 1
 * @{
 */
#ifndef CT16B1_PWM_H_
#define CT16B1_PWM_H_

void CT16B1_initpwm(uint16_t period_arg, uint16_t defaultstate, uint16_t prescaler_arg);
void CT16B1_reinitpwm();
void CT16B1_start(void);
void CT16B1_reload_mat(void);
void CT16B1_set(uint8_t matnr, uint16_t value);
void CT16B1_deinit(uint8_t state);
void CT16B1_wait_refresh(void);
uint16_t CT16B1_get_period(void);
uint32_t CT16B1_isStarted(void);

#endif //CT16B1_PWM_H_
/**
 *@}
 */
