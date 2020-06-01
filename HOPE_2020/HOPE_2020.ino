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

//variables globales para las pantallas (Inicialiamos valores por defecto)
int inspiratoryFlow = 30; //volumen a introducir en el paciente. Lo define el médico.. Rango de 0 - 50 (de 1 en 1) - default: 30
int flowPressure = 200; // aún no está integrada. Rango de 100 - 500 (de 10 en 10) - default: 200
int peep = 5; //no influye en el programa, pero tiene que poderse editar para que lo vea el médico. Rango de 0 - 25 (de 1 en 1) - default: 5
int piMax = 25; // aún no está integrada. Rango de 0 - 40 (de 1 en 1) - default: 25
int respiratoryRate = 30; //dato que introducirá el médico. Inspiraciones por minuto. Rango de 0 - 50 (de 1 en 1) - default: 15
int fi02 = 50; //no influye en el programa, pero tiene que poderse editar para que lo vea el médico. Rango de 0 a 100 (1 en 1) - default: 50
int ie = 3; //tasa inspiracion/espiracion: el paciente tarda 2 o 3 veces más en espirar que en inspirar. Lo define el mmédico
            // Rango de 1 - 3 (de 1 en 1) - default: 2

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



bool debug = false; //modo debug. Muestra mas logs por consola

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
        Serial.println("Nueva pagina cargada");
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

        case 5: //Pressure Control 1/2
            refreshCPAPSettings();
            break;

        case 6: //Pressure Control 2/2
            refreshCPAPDiag();
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

// Funciones refresco pantallas -----------------------------------------------------------------
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

        case 6: //CPAP 2/2
            refreshCPAPDiag();
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
    stop();
}

void refreshVolumeControlSettings() //Volume Control 1/2
{
    refreshSettingsScreenValues();
}

void refreshVolumeControlDiag() //Volume Control 2/2
{
    refreshDiagScreenValues();
}

void refreshPressureControlSettings() //Pressure Control 1/2
{
    refreshSettingsScreenValues();
}

void refreshPressureControlDiag() //Pressure Control 2/2
{
    refreshDiagScreenValues();
}

void refreshCPAPSettings() //CPAP 1/2
{
    refreshSettingsScreenValues();
}

void refreshCPAPDiag() //CPAP 2/2
{
    refreshDiagScreenValues();
}

// Pintado de valores en las pantallas tanto de settings como de diagnosis
void refreshSettingsScreenValues()
{
    myNex.writeNum("n0.val", inspiratoryFlow);  // Inspiratory flow
    myNex.writeNum("n1.val", flowPressure);     // Flow pressure
    myNex.writeNum("n2.val", peep);             // PEEP
    myNex.writeNum("n3.val", piMax);            // PiMax
    myNex.writeNum("n4.val", respiratoryRate);  // Respiratory Rate
    myNex.writeNum("n5.val", ie);               // IE Rate
    myNex.writeNum("n6.val", fi02);             // FiO2
}

void refreshDiagScreenValues()
{
    //4 valores fijos
    myNex.writeNum("n0.val", inspiratoryFlow);  // Inspiratory flow
    myNex.writeNum("n1.val", respiratoryRate);  // Respiratory rate
    myNex.writeNum("n2.val", flowPressure);     // Flow Pressure
    myNex.writeNum("n3.val", ie);               // IE

    //valores que fluctuan
    myNex.writeNum("n4.val", pnominal);         // Flow pressure instantanea
    myNex.writeNum("j0.val", pnominal);         // Flow pressure instantanea
    myNex.writeNum("n5.val", flow1);            // Inspiratory flow instantena
    myNex.writeNum("j1.val", flow1);            // Inspiratory flow instantena
}

// Control del motor -----------------------------------------------------------------
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
        Serial.println(caudal*respiratoryRate);
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

// Salvado e impresión de variables globales -----------------------------------------------------------------
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

// Triggers botones  -----------------------------------------------------------------

/* INFO: Para linkar un trigger con su botón hay que añadir en el releaseEvent del botón lo siguiente:
    Para el trigger1 printh 23 02 54 01
    Para el trigger2 printh 23 02 54 02
    ...
    Para el trigger10 printh 23 02 54 0A (OJO!! son valores hexadecimales)
*/

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

//PANTALLAS VOLUME Control (triggers de 11 a 20) ---------------------------
void trigger11() //Boton BACK Volume Control 1/2 (printh 23 02 54 0B)
{
    Serial.println("Pulsamos botón Back - Pagina Volume Control 1/2");
}

void trigger12() //Boton RUN Volume Control 1/2 (printh 23 02 54 0C)
{
    Serial.println("Pulsamos botón Run - Pagina Volume Control 1/2");
    saveValues();
    run();
}

void trigger13() //Boton BACK Volume Control 2/2 (printh 23 02 54 0D)
{
    Serial.println("Pulsamos botón Back - Pagina Volume Control 2/2");
}

//PANTALLAS Pressure Control (triggers de 21 a 30) -------------------------
void trigger21() //Boton BACK Pressure Control 1/2 (printh 23 02 54 15)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 1/2");
}

void trigger22() //Boton RUN Pressure Control 1/2 (printh 23 02 54 16)
{
    Serial.println("Pulsamos botón Run - Pagina Pressure Control 1/2");
    saveValues();
    run();
}

void trigger23() //Boton BACK Pressure Control 2/2 (printh 23 02 54 17)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 2/2");
}

//PANTALLAS CPAP (triggers de 31 a 40) -------------------------------------
void trigger31() //Boton BACK CPAP 1/2 (printh 23 02 54 1F)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 1/2");
}

void trigger32() //Boton RUN CPAP 1/2 (printh 23 02 54 20)
{
    Serial.println("Pulsamos botón Run - Pagina Pressure Control 1/2");
    saveValues();
    run();
}

void trigger33() //Boton BACK CPAP 2/2 (printh 23 02 54 21)
{
    Serial.println("Pulsamos botón Back - Pagina Pressure Control 2/2");
}