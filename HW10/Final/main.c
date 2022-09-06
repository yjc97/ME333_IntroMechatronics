#include "NU32.h"          // config bits, constants, funcs for startup and UART
// include other header files here
#include "encoder.h"
#include "ina219.h"
#include "utilities.h"
#include "position.h"

#include <stdio.h>
#include <xc.h>

#define BUF_SIZE 200


int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1; 
  set_mode(IDLE);       
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  UART2_Startup();
  INA219_Startup();
  Current_ISR_Startup();
  Position_ISR_Startup();
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a': //read current sensor (ADC counts)
      {
        int adc = INA219_read_current_adc();
        char m[50];
        sprintf(m, "%d\r\n", adc);
        NU32_WriteUART3(m);
        break;
      }
      
      case 'b': //read current sensor (mA)
      {
        float ma = INA219_read_current();
        char m[50];
        sprintf(m, "%f\r\n", ma);
        NU32_WriteUART3(m);
        break;
      }

      case 'c': //read encoder (counts)
      {
        WriteUART2("a");
        while(!get_encoder_flag()){}
        set_encoder_flag(0);
        char m[50];
        int p = get_encoder_count();
        sprintf(m, "%d\r\n",p);
        NU32_WriteUART3(m);
        break;
      }
      
      case 'd': // read encoder (degrees)
      {
        WriteUART2("a");
        while(!get_encoder_flag()){}
        set_encoder_flag(0);
        char m[50];
        int p = get_encoder_count();
        float d = p*360.0/(4*334.0);
        sprintf(m, "%f\r\n",d);
        NU32_WriteUART3(m);
        break;
      }

      case 'e': //reset encoder
      {
        WriteUART2("b");
        break;
      }

      case 'f': //set PWM (-100 to 100)
      {
        char m[50];
        NU32_ReadUART3(m, 50);
        int p;
        sscanf(m, "%d\r\n", &p);
        set_pwm(p);
        set_mode(PWM);
        sprintf(m, "%d\r\n",p);
        NU32_WriteUART3(m);
        break;
      }

      case 'g': //set current gains
      {
        char m[50];
        NU32_ReadUART3(m,50);
        float temp_kp, temp_ki;
        sscanf(m, "%f %f", &temp_kp, &temp_ki);
        set_current_gains(temp_kp, temp_ki);
        break;
      }

      case 'h': //get current gains
      {
        char m[50];
        sprintf(m, "%f %f\r\n", get_current_kp(), get_current_ki());
        NU32_WriteUART3(m);
        break;
      }

      case 'i': //set position gains
      {
        char m[50];
        NU32_ReadUART3(m,50);
        float temp_kp, temp_ki, temp_kd;
        sscanf(m, "%f %f %f", &temp_kp, &temp_ki, &temp_kd);
        set_position_gains(temp_kp, temp_ki, temp_kd);
        break;
      }

      case 'j': //get position gains
      {
        char m[50];
        sprintf(m, "%f %f %f\r\n", get_position_kp(), get_position_ki(), get_position_kd());
        NU32_WriteUART3(m);
        break;
      }

      case 'k': //test current control
      {
        set_mode(ITEST);
        while(get_mode()==ITEST){}
        ITEST_print();
        break;
      }

      case 'l': //go to angle (degrees)
      {
        
        break;
      }

      case 'm': //load step trajectory
      {
        char m[50];
        NU32_ReadUART3(m,50);
        int len = 0;
        sscanf(m, "%d",&len);
        set_desired_len(len);
        int i = 0;
        float pos = 0;
        for (i=0; i<len; i++){
          NU32_ReadUART3(m,50);
          sscanf(m, "%f", &pos);
          set_desired_position(i,pos);
        }
        break;
      }

      case 'n': //load cubic trajectory
      {
        char m[50];
        NU32_ReadUART3(m,50);
        int len = 0;
        sscanf(m, "%d",&len);
        set_desired_len(len);
        int i = 0;
        float pos = 0;
        for (i=0; i<len; i++){
          NU32_ReadUART3(m,50);
          sscanf(m, "%f", &pos);
          set_desired_position(i,pos);
        }
        break;
      }

      case 'o': //execute trajectory
      {
        set_mode(TRACK);
        while(get_mode()==TRACK){}
        TRACK_print();
        break;
      }

      case 'p': //unpower the motor
      {
        set_pwm(0);
        set_mode(IDLE);
        break;
      }

      case 'q': //quit client
      {
        // handle q for quit. Later you may want to return to IDLE mode here. 
        set_mode(IDLE);
        break;
      }

      case 'r': //get mode
      {
        char m[50];
        sprintf(m, "%d\r\n",get_mode());
        NU32_WriteUART3(m);
        break;
      }

      case 'x': // Test command
      { 
      int a = 0;
      int b = 0;
      NU32_WriteUART3("First number:\r\n");
      NU32_ReadUART3(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &a);
      NU32_WriteUART3("Second number:\r\n");
      NU32_ReadUART3(buffer, BUF_SIZE);
      sscanf(buffer, "%d", &b);
      sprintf(buffer, "%d\r\n", (a + b));
      NU32_WriteUART3("Sum:");
      NU32_WriteUART3(buffer);
      break;
      }

      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
