// Struct of data for contol the system.
enum t_motorState {STANDBY, INITIAL_POSITION, TURN_LEFT, TURN_RIGTH};
enum t_controlMode {VOLUME_CONTROL,  PRESSURE_CONTROL, TRIGGER, CPAP};

typedef  struct t_sensorsMeasure{
 float	temperature;
 float	pressure;
 float	inspirationVolume;
 bool	inspiratoryVolumeLimit;
 };

/*
typedef struct t_actuatorState{
  bool scapeValve;
  float motorAngle;
  }; 
 */

typedef struct t_displayParameters {
	float inspiratoryFlow; // Inspiratory flow
	float flowPressure;    // Blood pressure
	float peep;            // PEEP
	int piMax;           // PiMax
	int respiratoryRate; // Respiratory Rate
	int inspirationExpirationRate;              // IE Rate
	float fi02;            // FiO2*/
};

#ifdef DC_MOTOR
typedef struct t_DCMotor{
  t_motorState state;
  float wVelocity;
  int pwmTarget;
  }; 

typedef struct t_hopeSystem {
	bool upperEndOfCareerStatus;
	bool lowerEndOfCareerStatus;
	t_controlMode controlMode;
	t_DCMotor motorParameters;
	t_sensorsMeasure sensorsMeasure;
	t_displayParameters displayParameters;
};
#endif

// TODO: por implementar
#ifdef STEPPER_MOTOR
typedef struct t_StepperMotor {
	t_motorState state;
	float* speedRPM;
	float speedSteps;
	float maxSpeedSteps;
//	float wVelocity;
//	int pwmTarget;
};

typedef struct t_hopeSystem {
	bool upperEndOfCareerStatus;
	bool lowerEndOfCareerStatus;
	t_controlMode controlMode;
	t_StepperMotor motorParameters;
	t_sensorsMeasure sensorsMeasure;
	t_displayParameters displayParameters;
};

#endif
