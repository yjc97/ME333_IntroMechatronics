#ifndef POSITION__H__
#define POSITION__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

#include "NU32.h"
#include "utilities.h"
#include "ina219.h"
#include "current.h"
#include "encoder.h"

void Position_ISR_Startup();
void set_position_gains(float,float,float);
float get_position_ki();
float get_position_kp();
float get_position_kd();
void set_desired_position(int,float);
void set_desired_len(int);
void TRACK_print();

#endif // POSITION__H__