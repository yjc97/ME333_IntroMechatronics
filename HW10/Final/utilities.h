#ifndef utilities__H__
#define utilities__H__

enum mode_t{IDLE, PWM, ITEST, HOLD, TRACK};

volatile enum mode_t get_mode(void);
void set_mode(enum mode_t);

void set_pwm(int);
int get_pwm();

#endif