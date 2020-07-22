#include <String.h>

void Shell (void) {
  static String cmd;
  unsigned int strLength;
  cmd = Serial.readString();
  cmd.toUpperCase();
  Serial.println(cmd);
  //TODO: posible guardado de datos en la EEPROM.
  if (cmd == "HELP\n") {
    Serial.println(F("A continuación se muestrán los comandos aceptados que permiten modificar los parámetros del sistema."));
    Serial.println(F("Puesta en marcha: RUN"));
    /*Serial.println(F("            - W_XX YY"));
    Serial.println(F("            - W1_X YY"));
    Serial.println(F("            - W8_X YY"));
    Serial.println(F("EEPROM Save: Save"));
    Serial.println(F("Restaurar pesos por defecto: ResetWeigth"));*/
  // Comando SETWEIGTH W_11 34
  } else if (cmd == "RUN\n" /* && (cmd.length() >= 16)&&(cmd.length() <= 20)*/) {
    hopeSystem.motorParameters.state=INITIAL_POSITION;
    Serial.println(F("Cambio de estado a posición inicial."));
    Serial.println(hopeSystem.motorParameters.state);
    /*String weigthIndexStr = cmd.substring(13);
    String weigthValueStr = cmd.substring(15, cmd.length());
    int weigthIndex = weigthIndexStr.toInt();
    int weigthValue = weigthValueStr.toInt();*/
  /*} else if (cmd == "") {

  } else if (cmd == "") {

  } else if (cmd == ""){

    /********************************************************************/
  }  else {
    Serial.println(F("El comando introducido no es el correcto"));
  }
}
