#include "NU32.h"          // constants, functions for startup and UART

#define NUMSAMPS 1000 
#define PLOTPTS 200   

static volatile int state =0 ;
static volatile int Waveform[NUMSAMPS];
static volatile int ADCarray[PLOTPTS]; // store values of adc sample convert (15)
static volatile int OC1array[PLOTPTS]; // store values of OC1RS

void makeWaveform();
unsigned int adc_sample_convert(int pin);

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Timer2ISR(void) {  // INT step 1: the ISR
    
    static int counter = 0, plot = 0, batch = 0;
    static unsigned int sample15 = 0;

    OC1RS = Waveform[counter];
    sample15 = adc_sample_convert(15);

    if (state) {
      batch++;

      if (batch == 10) {
          batch = 0;                
          ADCarray[plot] = sample15; 
          OC1array[plot] = OC1RS;
          plot++; 
        }

       if (plot == PLOTPTS) { 
          plot = 0;            
          state = 0;        
        }
  

      counter++;

      if (counter == NUMSAMPS) {
        counter = 0;
        }

    IFS0bits.T2IF = 0;              // clear interrupt flag

  }
}

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init

  makeWaveform();
  AD1PCFGbits.PCFG15 = 0; // AN15 is an adc pin

  AD1CON1bits.SSRC = 0b111; // auto conversion
  AD1CON1bits.ASAM = 0;     // manual sampling

  AD1CON3bits.ADCS = 2;   // length of 
  AD1CON3bits.SAMC = 20;  // length of auto sampling

  
  AD1CON1bits.ADON = 1;   // turn on A/D converter

  __builtin_disable_interrupts(); // INT step 2: disable interrupts at CPU
                                  
  
  //timer3 20khz 
  T3CONbits.TCKPS = 1;      // Timer2 prescaler N=1 (1:4)
  
  PR3 = 3999;               // period = (PR2+1) * N * 12.5 ns = 100 us, 10 kHz
  TMR3 = 0;                 // initial TMR2 count is 0
  
  OC1CONbits.OCM = 0b110;   // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;    // enable timer3

  OC1RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;         // turn on Timer2
  OC1CONbits.ON = 1;        // turn on OC1
  
  
  //timer2 ISR 1khz
  T2CONbits.TCKPS = 1;            //             set prescaler to 256
  T2CONbits.TGATE = 0;            //             not gated input (the default)
  
  PR2 = 40999;                    //             set period register
  TMR2 = 0;                       //             initialize count to 0
  
  T2CONbits.ON = 1;               //             turn on Timer1
  IPC2bits.T2IP = 5;              // INT step 4: priority
  IPC2bits.T2IS = 0;              //             subpriority
  IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
  IEC0bits.T2IE = 1;              // INT step 6: enable interrupt
  
  __builtin_enable_interrupts();  // INT step 7: enable interrupts at CPU


  while(1) {
    
    __builtin_enable_interrupts();                      // infinite loop

    state = 1 ;
    
    while(state){
      ;
    }

  }

  return 0;
}

void makeWaveform() { 

  int i = 0, center = 1000, A = 500;
  for (i = 0; i < NUMSAMPS; ++i) {
    if (i < NUMSAMPS / 2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
    
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling

    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}