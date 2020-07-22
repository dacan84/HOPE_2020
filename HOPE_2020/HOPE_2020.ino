#include "EasyNextionLibrary.h" // Include EasyNextionLibrary
#include <Wire.h>
#include "pinout_declaration.h"
#include "dataTypes.h"
#include <String.h>
#include <EEPROM.h>

//#define SAVE_DEFAULT_VALUES 1

#define DEBUG
// Next defines selectr the functions implemented, mainly for the motor control. 
#define DC_MOTOR 
//#define STEPPER_MOTOR
#ifdef DC_MOTOR 
  #include <BTS7960.h>            //Driver motor
  BTS7960 motorController(MOTOR_EN, TURN_L_PWM, TURN_R_PWM);
#endif

#ifdef STEPPER_MOTOR 
    #include <AccelStepper.h>
    AccelStepper stepper = AccelStepper(MOTOR_INTERFACE_TYPE, STEP_PIN, DIRECTION_PIN);
#endif

#define INTERVAL_DATA_REFRESH_RATE 50 // Tiempo de refresco de cada pantalla. \
                                                     
#define INTERVAL_GET_DATA_EVERY 100 // Leemos los datos de los sensores cada X tiempo

EasyNex myNex(Serial1); // Create Nextion display Object and we send to constructor the Serial object requiered

unsigned long previousPageRefreshTimer = millis(); // Timer de DATA_REFRESH_RATE
bool newPageLoaded = false;                // true con la primera carga de una p�gina ( lastCurrentPageId != currentPageId )
unsigned long previousGetDataTimer = millis(); // Timer para GET_DATA_EVERY

t_hopeSystem hopeSystem;

//----------------------------------
// Inicializaci�n del cambio de p�gina
//----------------------------------

/* Para que se actualicen los valores de currentPageId, debemos escribir en el Preinitialize Event de cada p�gina: 
 *   `printh 23 02 50 XX` , donde `XX` es el id de la p�gina en HEX.  
 *      Para page0:  `printh 23 02 50 00`
 *      Para page9:  `printh23 02 50 09`
 *      Para page10: `printh 23 02 50 0A`
 */

void setup() {
   // put your setup code here, to run once:
#ifdef SAVE_DEFAULT_VALUES
    setDefaultValuesOnEEPROM();
#endif

    EEPROM.get(0,hopeSystem);
    //Nextion Screen
    Serial.begin(9600);
    myNex.begin(9600);        // equivale a Serial.begin(9600)
    delay(50);               // Damos tiempo a iniciar la pantalla
    myNex.writeStr(F("page 0")); // Por sincron�a por si se resetea el Arduino
    delay(50);
    myNex.lastCurrentPageId = 1; // al finalizar el primer loop, currentPageId y lastCurrentPageId deben ser distintos
    
#ifdef STEPPER_MOTOR
    stepper.setMaxSpeed(hopeSystem.motorParameters.maxSpeedSteps);
    stepper.setSpeed(hopeSystem.motorParameters.speedSteps);
#endif

    motorController.Enable();

    //LED
    pinMode(LED_GREEN,OUTPUT);
    pinMode(LED_RED,OUTPUT);

    //HONEYWELL sensor (sensor de volumen/caudalimetro)
    Wire.begin();
    
  #ifdef DEBUG
    Serial.println(F("arrancando el chisme"));;
#endif
    
}

void loop() {
  // put your main code here, to run repeatedly:
  #ifdef DEBUG
  while(Serial.available()!=0){
    Shell();
  }
  #endif
  
  Serial.println((int)hopeSystem.motorParameters.state);
  myNex.NextionListen(); // Debe ser llamada repetidamente para escuchar los eventos de la pantalla Nextion

    //readSensorValues();

    if (!firstRefresh())
    {
        refreshCurrentPage();
    }

    motorControl(&hopeSystem);
    inspirationVolume(&hopeSystem);
   
    if ((millis() - previousGetDataTimer) > INTERVAL_GET_DATA_EVERY) {
        //valores a leer
        readSensorValues(&hopeSystem);
        calculateSentorValues(&hopeSystem);
        previousGetDataTimer = millis();
    }

}

// esta función vale para guardar los valores por defecto en la eeprom.

void setDefaultValuesOnEEPROM(void){

t_hopeSystem defaulValues;
defaulValues.controlMode = VOLUME_CONTROL;
defaulValues.displayParameters.inspiratoryFlow = 30;
defaulValues.displayParameters.flowPressure = 200;
defaulValues.displayParameters.peep = 5;
defaulValues.displayParameters.piMax = 25;
defaulValues.displayParameters.respiratoryRate = 30;
defaulValues.displayParameters.inspirationExpirationRate = 3;
defaulValues.displayParameters.fi02 = 50;
defaulValues.sensorsMeasure.inspirationVolume = 0;
defaulValues.sensorsMeasure.inspiratoryVolumeLimit = LOW;
  
#ifdef DC_MOTOR
  defaulValues.motorParameters.state=STANDBY;
#endif
#ifdef STEPPER_MOTOR
  defaulValues.motorParameters.state=STANDBY;
#endif

EEPROM.put(0,defaulValues);
#ifdef DEBUG
  Serial.println(F("grabando datos por defecto en la eeprom"));
#endif
}

void readSensorValues(t_hopeSystem *hopeSystem) {// Aqu� pasaremos un puntero al sistema general hope o a los sensores. implementar en otro Fichero

    
}

void calculateSentorValues(t_hopeSystem* hopeSystem) {// Aqu� pasaremos un puntero al sistema general hope o a los sensores. implementar en otro Fichero

}
