#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "utilities.h"

#include <stdio.h>
#include <xc.h>


volatile enum mode_t mode;

int pwm = 0;

volatile enum mode_t get_mode(void){
  return mode;
}

void set_mode(enum mode_t s){
  mode=s;
}

void set_pwm(int p){
  pwm=p;
}

int get_pwm(){
  return pwm;
}