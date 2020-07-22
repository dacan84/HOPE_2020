#include "motorCtrl.h"
//#include "headers/dataTypes.h"

#ifdef DC_MOTOR
void motorControl(t_hopeSystem* hopeSystem) {
	static unsigned long previousmillisEndOfCarrer;
	
	switch (hopeSystem->motorParameters.state)
	{
	case INITIAL_POSITION:
		if (hopeSystem->upperEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
			hopeSystem->motorParameters.state=TURN_RIGTH;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnLeft(hopeSystem->motorParameters.pwmTarget/hopeSystem->displayParameters.inspirationExpirationRate); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;
	case TURN_LEFT:
		if (hopeSystem->sensorsMeasure.inspiratoryVolumeLimit == HIGH) {
			hopeSystem->motorParameters.state = TURN_RIGTH;
		} else if (hopeSystem->upperEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
				hopeSystem->motorParameters.state = TURN_RIGTH;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnLeft(hopeSystem->motorParameters.pwmTarget); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;

	case TURN_RIGTH:
		if (hopeSystem->lowerEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
				hopeSystem->motorParameters.state = TURN_LEFT;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnRight(hopeSystem->motorParameters.pwmTarget/hopeSystem->displayParameters.inspirationExpirationRate); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;

	case STANDBY:
		motorController.Stop();
	default:
		hopeSystem->motorParameters.state = STANDBY;
		motorController.Stop();
		break;
	}

}
#endif

#ifdef STEPPER_MOTOR //TODO: todav�a por implementar, copia del superior.
#include "headers/pinout_declaration.h"

void calculateSpeedStepper(t_hopeSystem* hopeSystem){
	hopeSystem->motorParameters.speedSteps = (float)(2 * (*speedRPM) * STEPS_PER_REVOLUCION) / minPerSecond;
	hopeSystem->motorParameters.maxSpeedSteps = (float)hopeSystem->motorParameters.speedSteps + 1000;
	}

void motorControl(t_hopeSystem* hopeSystem) {
	static unsigned long previousmillisEndOfCarrer;

	switch (hopeSystem->motorParameters.state)
	{
	case INITIAL_POSITION:
		if (hopeSystem->upperEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
				hopeSystem->motorParameters.state = TURN_RIGTH;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnLeft(hopeSystem->motorParameters.pwmTarget); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;
	case TURN_LEFT:
		if (hopeSystem->sensorsMeasure.inspiratoryVolumeLimit == HIGH) {
			hopeSystem->motorParameters.state = TURN_RIGTH;
		} else if (hopeSystem->upperEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
				hopeSystem->motorParameters.state = TURN_RIGTH;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnLeft(hopeSystem->motorParameters.pwmTarget); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;

	case TURN_RIGTH:
		if (hopeSystem->lowerEndOfCareerStatus == LOW) { // condicion de activaci�n del fin de carrera superior
			if (millis() - previousmillisEndOfCarrer > END_OF_CAREER_INTERVAL) {//
				hopeSystem->motorParameters.state = TURN_LEFT;
			}
		}
		else {
			previousmillisEndOfCarrer = millis();
		}
		motorController.TurnRight(hopeSystem->motorParameters.pwmTarget); // Sentido de giro para que el motor alcance su posici�n inicial.
		break;

	case STANDBY:
		motorController.Stop();
	default:
		hopeSystem->motorParameters.state = STANDBY;
		motorController.Stop();
		break;
	}

}

#endif 
