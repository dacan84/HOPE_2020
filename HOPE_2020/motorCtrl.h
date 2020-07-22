// motorCtrl.h

#ifndef _motorCtrl_h
#define _motorCtrl_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "pinout_declaration.h"
//#include "dataTypes.h"

// Next defines selectr the functions implemented, mainly for the motor control. 
//#define DC_MOTOR 
//#define STEPPER_MOTOR
#ifdef DC_MOTOR 
	#include <BTS7960.h>            //Driver motor
#endif

#ifdef STEPPER_MOTOR
	#include <AccelStepper.h> 
const float STEPS_PER_REVOLUCION = 3200;
const float minPerSecond = 60;

	AccelStepper stepper = AccelStepper(MOTOR_INTERFACE_TYPE, STEP_PIN, DIRECTION_PIN);
#endif

#define END_OF_CAREER_INTERVAL 100 // INTERVAL FOR ACTIVACION NEXT STATE WHEN AN END OF CAREER IS ACTIVATED
void motorControl(t_hopeSystem* hopeSystem);

void calculateSpeedStepper(t_hopeSystem* hopeSystem);

#endif

