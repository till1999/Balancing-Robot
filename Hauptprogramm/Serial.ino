
void printInfo(){
  //Alle 50 Millisekunden wird der Wert am seriellen Monitor ausgegeben
  if(millis() - lastTime >= 50){
    lastTime = millis();
    //UART.println(angleControlSetPoint);
    UART.print(pitchAngleGyro);
    UART.print("°");
    UART.print('\t');
    UART.print(pitchAngleAcc);
    UART.print("°");
    UART.print('\t');
    UART.print(pitchAngle);
    UART.println("°");
  }
}

void checkSerial(){
  receiveWithStartEndmarker();
  if(newData == true){
    strcpy(tempChars, receivedChars); //Kopiert Strings
    parseData();
    processParsedData();
    newData = false;
  }
}

void receiveWithStartEndmarker(){
  static bool receiveInProgress = false;
  static byte ndx = 0; //Stelle des Arrays an der operationen durchgeführt werden; Static: Variable bleibt nach Abschluss der Funktion erhalten
  char startMarker = '<'; //Start character am Anfang jedes Strings
  char endMarker = '>'; //Terminator character am Ende jedes Strings
  char rc; //Receive character
  while(UART.available() && newData == false){
    rc = UART.read();
    if(receiveInProgress == true){
      if(rc != endMarker){
        receivedChars[ndx] = rc;
        ndx++;
        if(ndx >= numChars){
          ndx = numChars - 1;
        }
      }else{
        receivedChars[ndx] = '\0';
        ndx = 0;
        newData = true;
        receiveInProgress = false;
      }
    }else if(rc == startMarker){
        receiveInProgress = true;
    }
  }
}

void parseData(){ //Teilt die Daten in ihre Einzelteile auf
  char * strtokIndex; //Pointer
  strtokIndex = strtok(tempChars, ","); //Wandelt Strings in Token um
  strcpy(message, strtokIndex);

  strtokIndex = strtok(NULL, ",");
  floatMessage = atof(strtokIndex);
}

void processParsedData(){
  if(strcmp(message,"wKP") == 0){ //strcmp vergleicht zwei strings miteinader
    UART.print("Ändere KP Wert zu: ");
    UART.println(floatMessage);
    angleControlKP = floatMessage;
    angleControl.SetTunings(angleControlKP,angleControlKI,angleControlKD); //Setzt neue Regelparameter
  }
  if(strcmp(message,"wKI") == 0){ //strcmp vergleicht zwei strings miteinader
    UART.print("Ändere KI Wert zu: ");
    UART.println(floatMessage);
    angleControlKI = floatMessage;
    angleControl.SetTunings(angleControlKP,angleControlKI,angleControlKD); //Setzt neue Regelparameter
  }
  if(strcmp(message,"wKD") == 0){ //strcmp vergleicht zwei strings miteinader
    UART.print("Ändere KD Wert zu: ");
    UART.println(floatMessage);
    angleControlKD = floatMessage;
    angleControl.SetTunings(angleControlKP,angleControlKI,angleControlKD); //Setzt neue Regelparameter
  }

  //Ein- und Ausschalten der Motoren
  if(strcmp(message,"en") == 0 && floatMessage == 1){ //strcmp vergleicht zwei strings miteinader
    UART.println("Schalte Motoren ein...");
    enabled = true;
  }
  if(strcmp(message,"en") == 0 && floatMessage == 0){ //strcmp vergleicht zwei strings miteinader
    UART.println("Schalte Motoren aus...");
    enabled = false;
    stopMotor();
  }
}
