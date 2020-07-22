#include <MapFloat.h>
#include "pinout_declaration.h"

#define INTERVAL_AIRFLOW_SUMATORY 10 // 10ms of measure interval
#define SIZE_NIM_INTEGRAL 5

const float SENSOR_TUBE_DIAMETER = 3.6/1000; //0
const float AIRFLOW_TIME_SCALE = 0.001; //s 
const float SENSOR_TUBE_RADIUS = SENSOR_TUBE_DIAMETER/2; //m
const float SENSOR_TUBE_SECTION = PI*SENSOR_TUBE_RADIUS*SENSOR_TUBE_RADIUS; //m^2

//TODO: define data pointer or array.

float analogReadAirFlowToFloat (int analogPin){
  float airFlow;
  //TODO: Maybe adjust the analog range for increase the meause range.
  airFlow = mapFloat(analogRead(PIN_AN_AIRFLOW),0,1023,0,2.7); // TODO: this sensor give a output voltage of 0-5 vots. We shoud adjust the calibration curve.
  return airFlow;
  }

float measAirFlowD6F(float *airFlowPrev){
  float airFlowIntegral;
  float currentAirFlow;
  static unsigned long previousAifFLowMeasureMillis = 0;
  unsigned long currentAifFLowMeasureMillis = millis();
 
  if (currentAifFLowMeasureMillis - previousAifFLowMeasureMillis >= INTERVAL_AIRFLOW_SUMATORY) {
    // actualize current air flow value
    previousAifFLowMeasureMillis = currentAifFLowMeasureMillis;
    currentAirFlow = analogReadAirFlowToFloat(PIN_AN_AIRFLOW);// TODO: adjust mapping range or make a manual adjust depending on the instrumentation amplifair output and GAIN 
  }

//TODO: to make the array copy for include the new measure and copy a memory area, 
  
}

float airFlorwConverter (float *airVelocity){
    float airFlow = 0;
    for (int i = 0; i>SIZE_NIM_INTEGRAL; i++){
      airFlow +=* airVelocity;
      ++airVelocity;
    }
    airFlow = airFlow/(INTERVAL_AIRFLOW_SUMATORY*AIRFLOW_TIME_SCALE);
    return airFlow;
  }
