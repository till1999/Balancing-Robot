
void initializePID(){
  angleControl.SetMode(AUTOMATIC); //PID Regler in automatischen Modus setzen
  angleControl.SetOutputLimits(angleControlMin,angleControlMax); //Grenzwerte der ausgegebenen Stellgröße (motorPWM) festlegen
  angleControl.SetSampleTime(sampleTime);
  wayControl.SetMode(AUTOMATIC); //PID Regler in automatischen Modus setzen
  wayControl.SetOutputLimits(wayControlMin,wayControlMax); //Grenzwerte der ausgegebenen Stellgröße (motorPWM) festlegen
  wayControl.SetSampleTime(sampleTime);
}
