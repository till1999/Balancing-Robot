
void initializeMotor(){
  //Pins initialisieren
  pinMode(AENABLE, OUTPUT);
  pinMode(APHASE, OUTPUT);
  pinMode(BENABLE, OUTPUT);
  pinMode(BPHASE, OUTPUT);
}

void moveMotor(int motorSpeed){
  if(motorSpeed > 0){ //Festlegen der Richtung in die der Motor drehen soll
    digitalWrite(APHASE, HIGH); 
    digitalWrite(BPHASE, LOW);
  }else{
    digitalWrite(APHASE, LOW); 
    digitalWrite(BPHASE, HIGH);
  }
  //Motorgeschwindigkeit schreiben
  analogWrite(AENABLE, (abs(motorSpeed) + motorAoffset));
  analogWrite(BENABLE, (abs(motorSpeed) + motorBoffset));
}

void stopMotor(){
  analogWrite(AENABLE, 0);
  analogWrite(BENABLE, 0);
}

void calculateWay(){
  encoderAposition = EncoderA.read();
  way = encoderAposition * 2 * M_PI * 0.035 / 100 / 12; //Berechnen des Weges: 0.035 = Radius des Rads in Metern; 100 = Übersetzung des Getriebes; 12 = Anzahl Counts pro Umdrehung der Magnetscheibe
  //UART.println(way);
}
