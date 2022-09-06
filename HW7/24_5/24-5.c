#include "NU32.h"
#include <stdio.h>


#define NUMSAMPS 1000 
#define PLOTPTS 200   // number of points in waveform
#define DECIMATION 10 // number of data points to plot

static volatile int Waveform[NUMSAMPS]; 
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData =   0; // if this flag = 1, currently storing plot data
static volatile float Kp = 0, Ki = 0; // control gains


void makeWaveform();
unsigned int adc_sample_convert(int pin);

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Timer2ISR(void) {
  static int counter = 0; // static- does not forget value when fucntion ends
  static int plotind = 0; // index for data arrays; counts up to PLOTPTS
  static int decctr = 0;  // counts to store data one every DECIMATION
  static int adcval = 0;

  OC1RS = Waveform[counter];

  //adcval = adc_sample_convert(15);
 
  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) {
      decctr = 0;                 // reset decimation counter
      //ADCarray[plotind] = adcval; // store data in global arrays
      REFarray[plotind] = Waveform[counter];
      plotind++; // increment plot data index
    }
    if (plotind == PLOTPTS) { // reach end of indices reset
      plotind = 0;            // reset the plot index
      StoringData = 0;        // tell main data is ready to be sent to MATLAB
    }
  }

  counter++;
  if (counter == NUMSAMPS) {
    counter = 0;
  }

  IFS0bits.T2IF = 0; // clear flag at end of interrupts
}

int main(void) {

  NU32_Startup(); 

  makeWaveform(); 
  // init adc
  AD1CON1bits.SSRC = 0x7; // Set the conversion process to be automatic
  AD1CON1bits.ASAM = 0;   // Set the sampling process to be manual
  AD1CON3bits.ADCS = 2;   // Set Tad to be 2*(ADCS + 1)* Tpb = 75 ns
  AD1CON3bits.SAMC = 20;  // Set sampling time to be 2*Tad = 150 ns
  AD1PCFGbits.PCFG15 = 0; // AN15 is an adc pin
  AD1CON1bits.ADON = 1;   // turn on A/D converter

  __builtin_enable_interrupts();
  // timer 3 20 kHz
  PR3 = 3999;
  TMR3 = 0;
  OC1CONbits.OCM = 0b110; // PWM mode with fault pin disabled
  OC1CONbits.OCTSEL = 1;  // user timer 3 for OC1
  OC1RS = 3000;
  OC1R = 3000;
  T3CONbits.ON = 1;
  OC1CONbits.ON = 1;      // turn OC1 on

  // timer 2 ISR 1 kHz ( interupt)
  T2CONbits.TCKPS = 1; 
  PR2 = 40999;
  TMR2 = 0;
  T2CONbits.ON = 1;
  IPC2bits.T2IP = 5;
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0;
  IEC0bits.T2IE = 1;
  __builtin_enable_interrupts();

  char message[100];            // message to and from MATLAB
  float kptemp = 0, kitemp = 0; // temporary local gains
  int i = 0;                    //  plot data loop counter

  while (1) {
    // wait for new gains
    NU32_ReadUART3(message,sizeof(message)); // wait till recieve gains from computer
    sscanf(message, "%f %f", &kptemp, &kitemp);
    __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
                                   // (turn off interupts )
    Kp = kptemp; // copy local variables to globals used by ISR
    Ki = kitemp;

    __builtin_enable_interrupts(); // turn interupts back on

    StoringData = 1; // messgae to ISR to start storing data

    while (StoringData) {
      ; // do nothing
    }

    for (i = 0; i < PLOTPTS; i++) {
      // when first number sent = 1 we're done
      sprintf(message, "%d %d %d\r\n", PLOTPTS - i, ADCarray[i],
              REFarray[i]); // index, number we read, duty cycle (eventually
                            // desired analog value)
      NU32_WriteUART3(message);
    }
  }

  return 0;
}

void makeWaveform() { // waveform with 1000 elements, first half 75% duty cycle,
                      // second half 25% duty cycle open loop control of light
                      // out of LED
  int i = 0, center = 600, A = 200;
  for (i = 0; i < NUMSAMPS; ++i) {
    if (i < NUMSAMPS / 2) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

unsigned int adc_sample_convert(int pin) {
  AD1CHSbits.CH0SA = pin; // set pin15
  AD1CON1bits.SAMP = 1;   // start sampling
  while (!AD1CON1bits.DONE) {
    ; 
  }
  return ADC1BUF0; // return result
}