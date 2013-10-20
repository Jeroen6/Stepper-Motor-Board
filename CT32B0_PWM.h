#ifndef CT32B0_PWM_H_
#define CT32B0_PWM_H_


#define STAGE_IOCON_GPIO
#define STAGE_IOCON_MAT

void CT32B0_initpwm(uint32_t period, uint32_t defaultstate);
void CT32B0_reinitpwm();
void CT32B0_start(void);
void CT32B0_stage(uint8_t stagearg);
void CT32B0_reload_mat(void);
void CT32B0_set(uint8_t matnr, uint32_t value);
void CT32B0_deinit(uint8_t state);
void CT32B0_wait_refresh(void);

#endif //CT32B0_PWM_H_
