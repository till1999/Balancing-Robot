void initializeButtons(){
  button1.attach(BUTTON1PIN, INPUT_PULLUP); //Mitteilen, an welchen Pins die Taster angeschlossen sind
  button2.attach(BUTTON2PIN, INPUT_PULLUP);
  button1.interval(5); //Debounce-Zeit festlegen
  button2.interval(5);
}

void checkButtons(){
  button1.update();
  button2.update();

  if(button1.fell()){
    pitchAngleOffset = pitchAngleOffset + pitchAngle; //Offset des Kippwinkels neu einstellen
    UART.print("Kippwinkel Offset auf ");
    UART.print(pitchAngleOffset);
    UART.println("° eingestellt");
  }
  
  if(button2.fell()){
    mode = (mode + 1) % numberModes; //%: "Modulo": Gibt den Rest einer Division aus; Formel ermöglicht ringförmiges Durchzählen einer Variable
    if(mode == 0){ //System ausgeschaltet
      UART.println("System Ausgeschaltet");
      enabled = false; //Motoren ausschalten
      stopMotor();
      digitalWrite(LED, LOW); //Status-LED ausschalten
    }
    if(mode == 1){
      UART.println("Winkelregelung eingeschaltet");
      enabled = true; //Motoren einschalten
      wayControlEnabled = false; //Wegregelung ausschalten
      angleControlSetPoint = 0; //Sollwert des Kippwinkels auf 0 setzen
    }
    if(mode == 2){
      UART.println("Weg- und Winkelregelung eingeschaltet");
      enabled = true; //Motoren einschalten
      wayControlEnabled = true; //Wegregelung einschalten
      wayControlSetPoint = way; //Neuen nullpunkt für die Wegregelung festlegen#
      digitalWrite(LED, HIGH); //Status-LED einschalten
    }
  }
  if(mode == 1 && millis() - lastLED >= 250){
    LEDstate = !LEDstate; //LED Zustand invertieren
    digitalWrite(LED, LEDstate);
    lastLED = millis(); //Timer zurücksetzen
  }
}
