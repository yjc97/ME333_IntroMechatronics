#include "position.h"
#include "encoder.h"
#include "ina219.h"
#include <stdio.h>

volatile float position_kp = 1000;
volatile float position_kd = 1000;
volatile float position_ki = 10;
#define EINTMAX 1000
volatile float d_pos[2000]; 
volatile float a_pos[2000]; 
volatile int d_len;

void set_position_gains(float kp, float ki, float kd){
    position_kp = kp;
    position_ki = ki;
    position_kd = kd;
}
float get_position_kp(){
    return position_kp;
}
float get_position_kd(){
    return position_kd;
}
float get_position_ki(){
    return position_ki;
}
void Position_ISR_Startup(){
    //timer 4 ISR 200 Hz
    T4CONbits.TCKPS = 3;    // prescaler N=8 (1:8)
    PR4 = 49999;
    TMR4 = 0;
    T4CONbits.ON = 1;
    IPC4bits.T4IP = 4;
    IFS0bits.T4IF = 0;
    IPC4bits.T4IS = 0;
    IEC0bits.T4IE = 1;
}
void __ISR(_TIMER_4_VECTOR, IPL4SOFT) Timer4ISR(void){
    IFS0bits.T4IF = 0;

    static int counter = 0;
    static float eint = 0;
    static float eprev = 0;

  switch(get_mode()){
    case TRACK:
    {
    WriteUART2("a");
    while(!get_encoder_flag()){}
    set_encoder_flag(0);
    float pos = get_encoder_count()*360.0/(4*334.0);
    a_pos[counter] = pos;
    float error = d_pos[counter] - pos;
    eint = eint + error;
    if (eint > EINTMAX){
        eint = EINTMAX;
      }
    if (eint < -EINTMAX){
        eint = -EINTMAX;
      }
    float edot = error - eprev;
    eprev = error;

    float u = position_kp*error + position_ki*eint +position_kd*edot;

    set_desired_torque(u);

    counter++;
    if (counter == d_len){
        counter = 0;
        eint = 0;
        eprev = 0;
        set_mode(IDLE);
      }
    break;
    }
  }
}

void set_desired_position(int index, float pos){
    if (index < 2000){
        d_pos[index] = pos;
    }
}

void set_desired_len(int len){
    d_len = len;
}

void TRACK_print(){
    char m[50];
    sprintf(m,"%d\r\n",d_len);
    NU32_WriteUART3(m);
    int i = 0;
    for(i=0; i<d_len; i++){
        sprintf(m,"%f %f\r\n", d_pos[i], a_pos[i]);
        NU32_WriteUART3(m);
    }
}