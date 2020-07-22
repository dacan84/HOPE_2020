#include "EasyNextionLibrary.h" // Include EasyNextionLibrary

extern t_hopeSystem hopeSystem;

bool firstRefresh()
{ // Está funcion sirve para actualizar los valores de una pantalla si es la primera vez que se carga

  if (myNex.currentPageId != myNex.lastCurrentPageId)
  { // Si los dos valores son diferentes, significa que se ha cargado una página nueva
    Serial.println(F("Nueva pagina cargada"));
    newPageLoaded = HIGH; // nueva página cargada

    switch (myNex.currentPageId)
    {
      case 0:
        refreshPage0(&hopeSystem.motorParameters);
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

    newPageLoaded = LOW;

    myNex.lastCurrentPageId = myNex.currentPageId; // Después de actualizar, igualamos los valores

    return HIGH;
  }

  return LOW;
}



// Funciones refresco pantallas -----------------------------------------------------------------
void refreshCurrentPage() //Actualizamos las páginas de monitorización (Diag)
{
  // En esta función actualizamos los valores cada DATA_REFRESH_RATE ms
  if ((millis() - previousPageRefreshTimer) > INTERVAL_DATA_REFRESH_RATE)
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
    previousPageRefreshTimer = millis();
  }
}

void refreshPage0(t_DCMotor *motor) //Pantalla inicial
{
#ifdef DEBUG
  Serial.println(F("Refrescamos página PRINCIPAL"));
#endif // DEBUG
  motor->state = STANDBY;
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
  myNex.writeNum("n0.val", hopeSystem.displayParameters.inspiratoryFlow);  // Inspiratory flow
  myNex.writeNum("n1.val", hopeSystem.displayParameters.flowPressure);     // Flow pressure
  myNex.writeNum("n2.val", hopeSystem.displayParameters.peep);             // PEEP
  myNex.writeNum("n3.val", hopeSystem.displayParameters.piMax);            // PiMax
  myNex.writeNum("n4.val", hopeSystem.displayParameters.respiratoryRate);  // Respiratory Rate
  myNex.writeNum("n5.val", hopeSystem.displayParameters.inspirationExpirationRate);               // IE Rate
  myNex.writeNum("n6.val", hopeSystem.displayParameters.fi02);             // FiO2

}

void refreshDiagScreenValues()
{
  //4 valores fijos
  myNex.writeNum("n0.val", hopeSystem.displayParameters.inspiratoryFlow);  // Inspiratory flow
  myNex.writeNum("n1.val", hopeSystem.displayParameters.respiratoryRate);  // Respiratory rate
  myNex.writeNum("n2.val", hopeSystem.displayParameters.flowPressure);     // Flow Pressure
  myNex.writeNum("n3.val", hopeSystem.displayParameters.inspirationExpirationRate);               // IE

  //valores que fluctuan
  /*   myNex.writeNum("n4.val", pnominal);         // Flow pressure instantanea
     myNex.writeNum("j0.val", pnominal);         // Flow pressure instantanea
     myNex.writeNum("n5.val", flow1);            // Inspiratory flow instantena // no comprendo la diferencia entre flujos de inspiración, el instantaneo vale realmente para algo?
     myNex.writeNum("j1.val", flow1);            // Inspiratory flow instantena*/
}

// Control del motor -----------------------------------------------------------------
void run()
{
  /*   Serial.println("Ejecutamos funcion run()");

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

         motorController.Stop(); //no ponemos motorOn a false porque no queremos dejar de llamar a run() en el loop.
     }

     Serial.print(caudal*respiratoryRate, DEC); // Print litres/hour
     Serial.println(" L/min");*/
}
/******  ESTA FUNCION YA NO ES NECESARIA  ******/
/*void stopByUser()
  {
    Serial.println("Ejecutamos función stopByUser()");
    motorController.Stop();
    motorOn = false;
  }*/

// Salvado e impresión de variables globales -----------------------------------------------------------------
void addDisplayValues(t_hopeSystem* hopeSystem)
{
#ifdef DEBUG
  Serial.println("Ejecutamos función addDisplayValues()");
#endif // DEBUG
  hopeSystem->displayParameters.inspiratoryFlow = myNex.readNumber("n0.val"); // Inspiratory flow
  hopeSystem->displayParameters.flowPressure = myNex.readNumber("n1.val");   // Blood pressure
  hopeSystem->displayParameters.peep = myNex.readNumber("n2.val");            // PEEP
  hopeSystem->displayParameters.piMax = myNex.readNumber("n3.val");           // PiMax
  hopeSystem->displayParameters.respiratoryRate = myNex.readNumber("n4.val"); // Respiratory Rate
  hopeSystem->displayParameters.inspirationExpirationRate = myNex.readNumber("n5.val");              // IE Rate
  hopeSystem->displayParameters.fi02 = myNex.readNumber("n6.val");            // FiO2*/
}

void printValues()
{
  /*#ifdef DEBUG
      Serial.println("===================================================================================");
      Serial.println(" Current Values: - InspiratoryFlow: " + String(inspiratoryFlow) + " - Blood pressure: " + String(flowPressure));
      Serial.println(" - Peep: " + String(peep) + " - PiMax: " + String(piMax) + " - Respiratory rate: " + String(respiratoryRate));
      Serial.println(" - IE: " + String(ie) + " - Fi02: " + String(fi02));
      Serial.println("===================================================================================");
  #endif // DEBUG*/
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
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Volume Control - Pagina principal"));
#endif // DEBUG
}

void trigger2() //Boton Pressure Control - Main screen (printh 23 02 54 02)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Pressure Control - Pagina principal"));
#endif // DEBUG
}

void trigger3() //Boton CPAP - Main screen (printh 23 02 54 03)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón CPAP  - Pagina principal"));
#endif // DEBUG

}

//PANTALLAS VOLUME Control (triggers de 11 a 20) ---------------------------
void trigger11() //Boton BACK Volume Control 1/2 (printh 23 02 54 0B)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Volume Control 1/2"));
#endif // DEBUG
  hopeSystem.motorParameters.state = STANDBY;
  //stopByUser(); //paramos motor al ir a la pantalla principal
}
void trigger12() //Boton RUN Volume Control 1/2 (printh 23 02 54 0C)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Run - Pagina Volume Control 1/2"));
#endif // DEBUG
  addDisplayValues(&hopeSystem);
  hopeSystem.motorParameters.state = INITIAL_POSITION;
  //motorOn = true; //arrancamos motor
}

void trigger13() //Boton BACK Volume Control 2/2 (printh 23 02 54 0D)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Volume Control 2/2"));
#endif // DEBUG
}

//PANTALLAS Pressure Control (triggers de 21 a 30) -------------------------
void trigger21() //Boton BACK Pressure Control 1/2 (printh 23 02 54 15)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Pressure Control 1/2"));
#endif // DEBUG
  hopeSystem.motorParameters.state = STANDBY;
  // stopByUser(); //paramos motor al ir a la pantalla principal
}

void trigger22() //Boton RUN Pressure Control 1/2 (printh 23 02 54 16)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Run - Pagina Pressure Control 1/2"));
#endif // DEBUG
  addDisplayValues(&hopeSystem);
  hopeSystem.motorParameters.state = INITIAL_POSITION;
  //motorOn = true; //arrancamos motor
}

void trigger23() //Boton BACK Pressure Control 2/2 (printh 23 02 54 17)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Pressure Control 2/2"));
#endif // DEBUG
}

//PANTALLAS CPAP (triggers de 31 a 40) -------------------------------------
void trigger31() //Boton BACK CPAP 1/2 (printh 23 02 54 1F)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Pressure Control 1/2"));
#endif // DEBUG
  hopeSystem.motorParameters.state = STANDBY;
  //stopByUser(); //paramos motor al ir a la pantalla principal
}

void trigger32() //Boton RUN CPAP 1/2 (printh 23 02 54 20)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Run - Pagina Pressure Control 1/2"));
#endif // DEBUG
  addDisplayValues(&hopeSystem);
  hopeSystem.motorParameters.state = INITIAL_POSITION;
  //motorOn = true; //arrancamos motor
}

void trigger33() //Boton BACK CPAP 2/2 (printh 23 02 54 21)
{
#ifdef DEBUG
  Serial.println(F("Pulsamos botón Back - Pagina Pressure Control 2/2"));
#endif // DEBUG
}
