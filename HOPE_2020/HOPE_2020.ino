#include "EasyNextionLibrary.h" // Include EasyNextionLibrary
#include "BTS7960.h"            //Driver motor
#include <Wire.h>

EasyNex myNex(Serial); // Creamos el objeto de la clase EasyNex y le pasamos el objeto Serial que vamos a usar

//led encendido
const int LEDVERDE=4;
const int LEDROJO=3;

//declaraciones findecarrera
int fcinferiorpin=7;
int fcinferiorstatus=0;
int fcsuperiorpin=6;
int fcsuperiorstatus=0;

//HONEYWELL ZEPHYR AIRFLOW (caudalimetro)
float flow1=0;
float caudal=0;

//sensor de presion
float pnominal=0;
float pnominalpin=11;

//variables globales para las pantallas
int inspiratoryFlow = 25; //volumen a introducir en el paciente. Lo define el médico
int flowPressure; // aún no está integrada
int peep; //no influye en el programa, pero tiene que poderse editar para que lo vea el médico
int piMax; // aún no está integrada
int respiratoryRate = 30; //dato que introducirá el médico. Inspiraciones por minuto 
int fi02; //no influye en el programa, pero tiene que poderse editar para que lo vea el médico
int ie = 3; //tasa inspiracion/espiracion: el paciente tarda 2 o 3 veces más en espirar que en inspirar. Lo define el mmédico

//declaraciones de PWM y velocidad
const uint8_t EN = 8;
const uint8_t L_PWM = 9;
const uint8_t R_PWM = 10;
BTS7960 motorController(EN, L_PWM, R_PWM);
int rpmmotor = 50;  //constante, velocidad nominal del motor DC (fabricante). Esto vale para convertir en PWM (0-255)
int wav =((ie*ie*respiratoryRate)/(ie+1)); //fórmula vel.angular de avance
int percent=wav*100/rpmmotor; //%de carga del motor en porcentaje
int percentarduino=percent*256/100; //% de carga del motor, en formato PWM (0-255)
int wre = wav/ie; //Vel.angular de retroceso
int diferencial =respiratoryRate*3;   //diferencial de tiempo. para que no se aturulle ARDUINO con la lectura de los fin de carrera. esto está dando problemas. Lo he definido por ensayo-error. no es 100%fiable
float currentTime;//UN CONTADOR DE TIEMPO 
float currentTimedos; //otro contador de tiempo. La resta de contadores es el Tiempo entre lecturas para hacer el sumatorio de cuanto aire llevamos metido en cada inspiracion.



bool debug = true; //modo debug

//----------------------------------
// Inicialización del cambio de página
//----------------------------------

/* Para que se actualicen los valores de currentPageId, debemos escribir en el Preinitialize Event de cada página: 
 *   `printh 23 02 50 XX` , donde `XX` es el id de la página en HEX.  
 *      Para page0:  `printh 23 02 50 00`
 *      Para page9:  `printh23 02 50 09`
 *      Para page10: `printh 23 02 50 0A`
 */
#define DATA_REFRESH_RATE 50 // Tiempo de refresco de cada pantalla. \
                             // Delay en la ejecución del loop

unsigned long pageRefreshTimer = millis(); // Timer de DATA_REFRESH_RATE
bool newPageLoaded = false;                // true con la primera carga de una página ( lastCurrentPageId != currentPageId )

#define GET_DATA_EVERY 2000 // Leemos los datos de los sensores cada X tiempo

unsigned long getDataTimer = millis(); // Timer para GET_DATA_EVERY

void setup()
{
    //Nextion pantalla
    myNex.begin(9600);        // equivale a Serial.begin(9600)
    delay(500);               // Damos tiempo a iniciar la pantalla
    myNex.writeStr("page 0"); // Por sincronía por si se resetea el Arduino
    delay(50);
    myNex.lastCurrentPageId = 1; // al finalizar el primer loop, currentPageId y lastCurrentPageId deben ser distintos

    currentTime = millis(); //UN CONTADOR DE TIEMPO PARA WHATEVER  
    motorController.Enable();

    //parte del led
    pinMode(LEDVERDE,OUTPUT);
    pinMode(LEDROJO,OUTPUT);

    //parte del HONEYWELL (sensor de volumen/caudalimetro)
    Wire.begin();
}

void loop()
{

    myNex.NextionListen(); // Debe ser llamada repetidamente para escuchar los eventos de la pantalla Nextion

    readSensorValues();

    if (!firstRefresh())
    {
        refreshCurrentPage();
    }
}

bool firstRefresh()
{ // Está funcion sirve para actualizar los valores de una pantalla si es la primera vez que se carga

    if (myNex.currentPageId != myNex.lastCurrentPageId)
    { // Si los dos valores son diferentes, significa que se ha cargado una página nueva

        newPageLoaded = true; // nueva página cargada

        switch (myNex.currentPageId)
        {
        case 0:
            refreshPage0();
            break;

        case 1: //Volume Control 1
            refreshVolumeControlSettings();
            break;

        case 2: //Volume Control 2/2
            refreshVolumeControlDiag();
            break;

        case 3: //Pressure Control 1/2
            refreshPressureControlSettings();
            break;

        case 4: //Pressure Control 2/2
            refreshPressureControlDiag();
            break;
        }

        newPageLoaded = false;

        myNex.lastCurrentPageId = myNex.currentPageId; // Después de actualizar, igualamos los valores

        return true;
    }

    return false;
}

void readSensorValues()
{
    if ((millis() - getDataTimer) > GET_DATA_EVERY)
    {
        //valores a leer

        getDataTimer = millis();
    }
}

void refreshCurrentPage() //Actualizamos las páginas de monitorización (Diag)
{
    // En esta función actualizamos los valores cada DATA_REFRESH_RATE ms
    if ((millis() - pageRefreshTimer) > DATA_REFRESH_RATE)
    {
        switch (myNex.currentPageId)
        {
        case 2: //Volume Control 2/2
            refreshVolumeControlDiag();
            break;

        case 4: //Pressure Control 2/2
            refreshPressureControlDiag();
            break;

        default:
            break;
        }
        pageRefreshTimer = millis();
    }
}

void refreshPage0() //Pantalla inicial
{
    Serial.println("Refrescamos página PRINCIPAL");
}

void refreshVolumeControlSettings() //Volume Control 1/2
{
    Serial.println("Cargamos valores página VOLUME Control 1/2");

    stop();
    myNex.writeNum("n0.val", inspiratoryFlow); // Inspiratory flow
    myNex.writeNum("n1.val", flowPressure);   // Blood pressure
    myNex.writeNum("n2.val", peep);            // PEEP
    myNex.writeNum("n3.val", piMax);           // PiMax
    myNex.writeNum("n4.val", respiratoryRate); // Respiratory Rate
    myNex.writeNum("n5.val", ie);              // IE Rate
    myNex.writeNum("n6.val", fi02);            // FiO2
}

void refreshVolumeControlDiag() //Volume Control 2/2
{
    Serial.println("Actualizamos valores página VOLUME Control 2/2");
    run();
}

void refreshPressureControlSettings() //Pressure Control 1/2
{
    Serial.println("Cargamos valores página PRESSURE Control 1/2");
    stop();

    myNex.writeNum("n0.val", inspiratoryFlow); // Inspiratory flow
    myNex.writeNum("n1.val", flowPressure);   // Blood pressure
    myNex.writeNum("n2.val", peep);            // PEEP
    myNex.writeNum("n3.val", piMax);           // PiMax
    myNex.writeNum("n4.val", respiratoryRate); // Respiratory Rate
    myNex.writeNum("n5.val", ie);              // IE Rate
    myNex.writeNum("n6.val", fi02);            // FiO2
}

void refreshPressureControlDiag() //Pressure Control 2/2
{
    Serial.println("Actualizamos valores página PRESSURE Control 2/2");
    run();
}

//Arranque motorController
void run()
{
    Serial.println("Ejecutamos funcion run()");

    currentTime=millis();
    delay (diferencial);
    Wire.requestFrom(0x49, 2);
    byte MostSigByte = Wire.read(); // Read the first byte this is the MSB
    byte LeastSigByte = Wire.read(); // Now Read the second byte this is the LSB 43
    unsigned int PressSum = (((MostSigByte << 8) + LeastSigByte));                    //formulas OK???
    float flow = (200*((PressSum/16384.0)-.1)/.8);                    //me salen litros/min
    flow1=flow/60;                                                    //me salen litros/sg
    currentTimedos=millis();
    flow1=flow1/1000*(currentTimedos-currentTime);                    //hago el diferencial de caudal en funcion de los millis empleados. YA TENGO DIFERENCIAL DE VOLUMEN (litros)
    caudal=caudal+flow1;                                              //sumo los diferenciales, tengo un VOLUMEN unitario (litros/inspiración)
    //flow1= flow1+flow;  
    pnominal=digitalRead(pnominalpin);  
    motorController.TurnLeft(percentarduino);
    digitalWrite(LEDVERDE,HIGH);
    digitalWrite(LEDROJO,LOW);
    fcsuperiorstatus=digitalRead(fcsuperiorpin);
   
    if (fcsuperiorstatus == 1 || caudal * respiratoryRate > inspiratoryFlow)      //RECORRIDO HACIA ATRÁS
    { 
        Serial.print("Paramos a (L/min):");
        Serial.println(caudal*fr);
        //motorController.Stop();
        caudal=0;
        flow1=0;
        flow=0;

        do {
            delay(1);
            motorController.TurnRight(percentarduino/ie);
            digitalWrite(LEDVERDE,LOW);
            digitalWrite(LEDROJO,HIGH);
            fcinferiorstatus=digitalRead(fcinferiorpin);
            fcsuperiorstatus=digitalRead(fcsuperiorpin); 
        } while(fcinferiorstatus==0); //debiera ser 1, pero ruido

        stop();
    }

    Serial.print(caudal*respiratoryRate, DEC); // Print litres/hour
    Serial.println(" L/min");
}

void stop()
{
    Serial.println("Ejecutamos función stop()");

    //TO DO - Parar motor
    motorController.Stop();
}

void saveValues()
{
    Serial.println("Ejecutamos función saveValues()");
    inspiratoryFlow = myNex.readNumber("n0.val"); // Inspiratory flow
    flowPressure = myNex.readNumber("n1.val");   // Blood pressure
    peep = myNex.readNumber("n2.val");            // PEEP
    piMax = myNex.readNumber("n3.val");           // PiMax
    respiratoryRate = myNex.readNumber("n4.val"); // Respiratory Rate
    ie = myNex.readNumber("n5.val");              // IE Rate
    fi02 = myNex.readNumber("n6.val");            // FiO2
}

void printValues()
{
    if (debug) {
        Serial.println("===================================================================================");
        Serial.println(" Current Values: - InspiratoryFlow: " + String(inspiratoryFlow) + " - Blood pressure: " + String(flowPressure));
        Serial.println(" - Peep: " + String(peep) + " - PiMax: " + String(piMax) + " - Respiratory rate: " + String(respiratoryRate));
        Serial.println(" - IE: " + String(ie) + " - Fi02: " + String(fi02));
        Serial.println("===================================================================================");
    }  
}

/** -------- Triggers botones -------- **/
// INFO: Para linkar un trigger con su botón hay que añadir en el releaseEvent del botón lo siguiente:
// Para el trigger1 printh 23 02 54 01
// Para el trigger2 printh 23 02 54 02
// ...
// Para el trigger10 printh 23 02 54 0A (OJO!! son valores hexadecimales)

// PANTALLA PRINCIPAL (triggers de 1 a 10)
void trigger1() //Boton Volume Control - Main screen (printh 23 02 54 01)
{
    Serial.println("Pulsamos botón Volume Control - Pagina principal");
}

void trigger2() //Boton Pressure Control - Main screen (printh 23 02 54 02)
{
    Serial.println("Pulsamos botón Pressure Control - Pagina principal");
}

void trigger3() //Boton CPAP - Main screen (printh 23 02 54 03)
{
    Serial.println("Pulsamos botón CPAP  - Pagina principal");

}

//PANTALLAS VOLUME Control (triggers de 11 a 20)
void trigger11() //Boton BACK Volume Control 1/2 (printh 23 02 54 0B)
{
    Serial.println("Pulsamos botón Back - Pagina Volume Control 1/2");
}

void trigger12() //Boton RUN Volume Control 1/2 (printh 23 02 54 0C)
{
    Serial.println("Pulsamos botón Run - Pagina Volume Control 1/2");
    saveValues();
}

void trigger13() //Boton BACK Volume Control 2/2 (printh 23 02 54 0D)
{
    Serial.println("Pulsamos botón Back - Pagina Volume Control 2/2");
}

//PANTALLAS Pressure Control (triggers de 21 a 30)
void trigger21() //Boton BACK Pressure Control 1/2 (printh 23 02 54 15)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 1/2");
}

void trigger22() //Boton RUN Pressure Control 1/2 (printh 23 02 54 16)
{
    Serial.println("Pulsamos botón Run - Pagina Pressure Control 1/2");
    saveValues();
}

void trigger23() //Boton BACK Pressure Control 2/2 (printh 23 02 54 17)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 2/2");
}

//PANTALLAS CPAP
