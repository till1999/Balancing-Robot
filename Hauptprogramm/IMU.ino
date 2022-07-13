
void calculateAngle(){ //Berechnet den Kippwinkel aus der IMU
  //Bewegungsdaten von der IMU abfragen:
  imu.getMotion6(&axRaw, &ayRaw, &azRaw, &gxRaw, &gyRaw, &gzRaw); // "&": Funktion ändert den Wert der Variable selbst, erstellt keine interne Kopie der Werte 

  //Winkel aus Rohdaten berechnen
  pitchAngleAcc = (atan2(azRaw, sqrt(ayRaw*ayRaw+axRaw*axRaw)) * 180 / M_PI) - pitchAngleOffset; //atan2 erwartet zwei Werte: Den Zähler & Nenner eines Bruchs; Formel: "Euler-Winkel"; *180 / M_PI: Umrechnen Rad in Deg

  //Winkelgeschwindigkeit ausrechnen: gyDot = gyRaw * Range der IMU [°/s] / Range der IMU [2^16 Werte] => Analog dem Map Befehl 
  gyDot = gyRaw * 2000.0f / 65536.0f; //f macht aus der 500 eine Float zahl
  
  currentTime = millis();
  gyroDt = currentTime - gyroTime;
  pitchAngleGyro = pitchAngle - gyDot * (gyroDt/1000.0f);
  gyroTime = currentTime;

  pitchAngle = (compWeight * pitchAngleAcc + (1-compWeight) * pitchAngleGyro); //Komplementärfilter 

}

void initializeIMU(){ //IMU intitialisieren
  UART.println("Initialisiere IMU....");
  imu.initialize();
  UART.println("Testen ob IMU verbunden....");
  UART.println(imu.testConnection() ? "IMU verbindung erfolgreich." : "IMU verbindung fehlgeschlagen."); // "?" ist kurzschreibweise für if-Abfrage; ":" ist kurzschreibweise für else-Kommando 
  //IMU einstellen und kalibrieren
  imu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
  imu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  UART.println("Sensorversatz einstellen: Zum neu kalibrieren Beispiele -> IMU6050 -> IMU_Zero ausführen!");
  UART.println();
  UART.println("*************************************");
  UART.println();
  imu.setXAccelOffset(xAccelOffset);
  imu.setYAccelOffset(yAccelOffset);
  imu.setZAccelOffset(zAccelOffset);
  imu.setXGyroOffset(xGyroOffset);
  imu.setYGyroOffset(yGyroOffset);
  imu.setZGyroOffset(zGyroOffset); 

  //Kippwinkel Anfangswert bestimmen:
  imu.getMotion6(&axRaw, &ayRaw, &azRaw, &gxRaw, &gyRaw, &gzRaw);
  pitchAngle = atan2(azRaw, sqrt(ayRaw*ayRaw+axRaw*axRaw)) * 180 / M_PI;
}
