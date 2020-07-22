/*********** Define MOTOR Type ************/

#define DC_MOTOR 1
//#define STEPPER_MOTOR


/*********** PINES ASOCIADOS A LOS SENSORES ANALÓGICOS ****************/
#define PIN_AN_PREASURE 22
#define PIN_AN_AIRFLOW A0 // pin defined for measure de air flow

#define	FLOW_SENSOR_ADDRESS			0x49
#define FLOW_SENSOR_DATA_LENGTH		2

/********* End of Career Pins Declarations **********/
#define PIN_UPPER_END_OF_CAREER 6
#define PIN_LOWER_END_OF_CAREER 7

/*********** PINES ASOCIADOS ****************/
//#define PIN_LDC_OUT  31 

/************** DC Motor pin declaration *********************/
//#define DC_MOTOR
#ifdef DC_MOTOR
	#define MOTOR_EN    8
	#define TURN_L_PWM  9
	#define TURN_R_PWM  10
#endif

/************** DC Motor pin declaration *********************/
//#define STEPPER_MOTOR
#ifdef STEPPER_MOTOR
	#define MOTOR_EN				8
	#define DIRECTION_PIN			6
	#define STEP_PIN				7
	#define MOTOR_INTERFACE_TYPE	1
#endif


/********** TEST PINS *************/
#define LED_GREEN 4
#define LED_RED  3
