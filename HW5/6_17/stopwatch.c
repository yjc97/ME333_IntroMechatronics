#include "NU32.h"          // constants, funcs for startup and UART
#include <stdio.h>

volatile int state = 0;
static volatile unsigned int  elapsed;
static volatile unsigned int  elapsed_second;

void __ISR(_EXTERNAL_0_VECTOR, IPL6SRS) Ext0ISR(void) { // step 1: the ISR
    char m[100];
    IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
    elapsed = _CP0_GET_COUNT();
    _CP0_SET_COUNT(0);  

    while(_CP0_GET_COUNT() < 2000000) { ; } // delay for 2 M core ticks, 0.05 s
    if(PORTDbits.RD7 == 0){

        
        
        if(state ==0){
            state = 1;
            NU32_WriteUART3("Press USER button to stop the timer\r\n");    
        }
        else if(state ==1){
            state = 0;
            elapsed_second = elapsed/40000000;
            sprintf(m,"%d seconds elapsed.\r\n\n",elapsed_second);
            NU32_WriteUART3(m);
            NU32_WriteUART3("Press USER button to start the timer\r\n");
            
        }
        
    }
}

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  __builtin_disable_interrupts(); // step 2: disable interrupts
 


  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 6;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 0;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)
  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}