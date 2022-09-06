#include "current.h"
#include "ina219.h"
#include <stdio.h>

volatile float current_kp = -0.208;
volatile float current_ki = -0.01;
#define EINTMAX 1000
volatile float d_ma[2000]; // ITEST desired current
volatile float a_ma[2000]; // ITEST actual current
volatile float desired_torque = 0;

void set_desired_torque(float u){
    desired_torque = u;
}

void set_current_gains(float kp, float ki){
    current_kp = kp;
    current_ki = ki;
}

float get_current_kp(){
    return current_kp;
}

float get_current_ki(){
    return current_ki;
}

void Current_ISR_Startup(){
    // timer 3 20k HZ
    //oc1 using timer 3
    T3CONbits.TCKPS = 1; //prescaler N = 2 (1:2)
    PR3 = 1999;
    TMR3 = 0;
    OC1CONbits.OCM = 0b110;
    OC1CONbits.OCTSEL = 1; // user timer 3
    OC1RS =0;
    OC1R = 0;
    T3CONbits.ON = 1;
    OC1CONbits.ON = 1;

    // dir pin on D1
    TRISDbits.TRISD1 = 0;
    LATDbits.LATD1 = 0;

    //timer 2 ISR 5kHZ
    T2CONbits.TCKPS = 2;  //prescaler N=4 (1:4)
    PR2 = 3999;
    TMR2 = 0;
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 6;
    IPC2bits.T2IS = 0;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE =1;
}

void __ISR(_TIMER_2_VECTOR, IPL6SRS) Timer2ISR(void){
    IFS0bits.T2IF = 0;

    static int counter = 0;
    static float desired_ma = 200.0;
    static float eint = 0;
    switch(get_mode()){
        case IDLE:
        {
            OC1RS = 0;
            LATDbits.LATD1 = 0;
            break;
        }
        case PWM:
        {
            int p = get_pwm();
            if ( p > 0){
                OC1RS = 1999*p/100;
                LATDbits.LATD1 = 1;
            }
            else {
                p = -p;
                OC1RS = 1999*p/100;
                LATDbits.LATD1 = 0;
            }
            break;
        }
        case ITEST:
        {
            if (counter == 25){
                desired_ma = -200.0;
            }
            if (counter == 50){
                desired_ma = 200.0;
            }
            if (counter == 75){
                desired_ma = -200.0;
            }
            float ma = INA219_read_current();
            float error = desired_ma - ma;
            eint = eint + error;
            if (eint > EINTMAX){
             eint = EINTMAX;
            }
            if (eint < -EINTMAX){
             eint = -EINTMAX;
            }
            float u = current_kp*error +current_ki*eint;
            if(u > 100.0){
             u = 100.0;
            }
            if (u < -100.0){
             u = -100.0;
            }
            if (u > 0){
                OC1RS = 1999*u/100;
                LATDbits.LATD1 = 1;
            }
            else{
                u = -u;
                OC1RS = 1999*u/100;
                LATDbits.LATD1 = 0;
            }
            d_ma[counter] = desired_ma;
            a_ma[counter] = ma;

            counter++;
            if (counter == 100){
                counter = 0;
                desired_ma = 200.0;
                eint = 0;
                set_mode(IDLE);
            }
            break;
        }
        case TRACK:
        {
            float ma = INA219_read_current();
            float error = desired_torque - ma;
            eint = eint +error;
            if (eint > EINTMAX){
                eint = EINTMAX;
            }
            if (eint < -EINTMAX){
                eint = -EINTMAX;
            }
            float u = current_kp*error + current_ki*eint;

            if(u > 100.0){
                u = 100.0;
            }
            if (u < -100.0){
                u = -100.0;
            }

            if ( u > 0){
                OC1RS = 1999*u/100;
                LATDbits.LATD1 = 1;
            } 
            else {
                u = -u;
                OC1RS = 1999*u/100;
                LATDbits.LATD1 = 0;
            }
            break;
    }
  }
}
void ITEST_print(){
    char m[50];
    NU32_WriteUART3("100\r\n");
    int i = 0;
    for (i=0;i<100;i++){
        sprintf(m,"%f %f\r\n", d_ma[i], a_ma[i]);
        NU32_WriteUART3(m);
    }
}