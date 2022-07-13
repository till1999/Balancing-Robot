//Libraries einbinden
#include "I2Cdev.h"
#include "MPU6050.h"
#include "wire.h"
#include <PID_v1.h>
#include <Encoder.h>
#include <Bounce2.h>

//Pindefinitionen
#define AENABLE 9 //PWM SIgnal für die Motorgeschwindigkeit
#define APHASE 8 //HIGH, LOW bestimmt Drehrichtung des Motors
#define BENABLE 6 //PWM SIgnal für die Motorgeschwindigkeit
#define BPHASE 7 //HIGH, LOW bestimmt Drehrichtung des Motors

#define ENCODERA1 2
#define ENCODERA2 3
#define ENCODERB1 4
#define ENCODERB2 5

#define BUTTON1PIN 14
#define BUTTON2PIN 15

#define LED 13

#define UART Serial1 //Definiere über welche Serielle Schnittstelle die Daten gesendet werden sollen 

//***Taster***
Button button1 = Button(); //Button-objekte anlegen
Button button2 = Button();

//***Motoren***
double motorPWM = 0; //Geschwindigkeit der Motoren, Stellgröße des Kippwinkel-PID-Reglers
int motorDirection;

int motorAoffset = 16;
int motorBoffset = 13;

int encoderAposition; //Position des Encoders des Motor A
double way; //Weg den der Segway gefahren ist
Encoder EncoderA(ENCODERA1,ENCODERA2);//Encoder Objekte Anlegen
Encoder EncoderB(ENCODERB1,ENCODERB2);


//***IMU***

MPU6050 imu; //Objekt der Instanz MPU6050 deklarieren.

//Offsetwerte aus der IMU Sensorkalibrierung:
int16_t xAccelOffset = -2935, yAccelOffset = 985, zAccelOffset = 1671;
int16_t xGyroOffset = 104, yGyroOffset = -14, zGyroOffset = -40;

int16_t axRaw, ayRaw, azRaw; //Variablen zum Speichern der Roh-beschleunigungswerte
int16_t gxRaw, gyRaw, gzRaw; //Variablen zum Speichern der Roh-lagewerte
 
float gyDot; //Variablen zum Speichern der Geschwindigkeit um die Y-Achse

float pitchAngleAcc; //Variabel zum Speichern des Winkels um die X-Achse welcher aus den Roh-Beschleungigungswerten berechnet wird
float pitchAngleGyro = 0; //Variabel zum Speichern des Winkels um die X-Achse welcher aus den Gyroskopwerten berechnet wird
double pitchAngle; //Variable speichert den Winkel, welcher mittels komplementärfilter aus dem Accelerometer & Gyroskop gewonnen wird; Regelgröße des Kippwinkel PID-Reglers

double pitchAngleOffset = -1.8; //Offset der IMU-Messung zum realen Winkel

float compWeight = 0.01; //Variable definiert wie sehr Accelerometer und Gyroskop bei der Winkelberechnung gewichtet werden

//***PID-Regler***
int sampleTime = 10; //Wie oft der PID-Algorithmus ausgewertet wird (in ms)

//Winkel PID
int angleControlMin = -255, angleControlMax = 255; ////Grenzwerte der ausgegebenen Stellgröße (motorPWM)
double angleControlSetPoint = 0; //Sollwert Kippwinkel
double angleControlKP = 10, angleControlKI = 200, angleControlKD = 0.2; //Tuning Parameter des PID Reglers
PID angleControl(&pitchAngle, &motorPWM, &angleControlSetPoint, angleControlKP, angleControlKI, angleControlKD, DIRECT); //Erstellen des PID-Regler Objekts; &-Symbol hängt Variable an das Objekt an

//Weg PID
int wayControlMin = -200, wayControlMax = 200; ////Grenzwerte der ausgegebenen Stellgröße (motorPWM)
double wayControlSetPoint = 0; //Sollwert Kippwinkel
double wayControlKP = 9, wayControlKI = 0, wayControlKD = 5; //Tuning Parameter des PID Reglers
PID wayControl(&way, &angleControlSetPoint, &wayControlSetPoint, wayControlKP, wayControlKI, wayControlKD, DIRECT); //Erstellen des PID-Regler Objekts; wayControlSetPoint ist die Regelgröße und Eingangsgröße des Winkel PID-Reglers

//***Systemzustand***
int mode = 2; //Variable legt den Betriebsmodus des Systems fest
int numberModes = 3; //Anzahl unterschiedlicher Betriebsmodi
bool enabled = true; //Legt fest ob Motoren ein- oder ausgeschaltet sind
bool wayControlEnabled = true; //Legt fest ob die Wegregelung ein- oder ausgeschaltet ist
bool LEDstate = false; //Speichert den Zustand der eingebauten LED

//***Timing***
long currentTime = 0; //Variable zum Speichern der aktuellen Zeit 
long lastTime = 0; //Variablen zum Speichern des Zeitpunkts der letzten Seriellen-Ausgabe
long gyroTime = 0; //Zeit seit der letzten Messung des Gyro-Wertes
long gyroDt = 0; //Zeitinterval
long lastLED = 0; //Zeitpunkt des letzten LED umschaltens


//***Serielle Kommunikation
const byte numChars = 32; //Länge des erlaubten Strings
char receivedChars[numChars]; //Variable speichert über UART11 gesendete Daten
char tempChars[numChars];

char message[numChars];
float floatMessage = 0.0;

bool newData = false;


void setup() {
  Wire.begin(); //I2C initialisieren
  UART.begin(9600); //Serielle Schnittstelle initialisieren
  pinMode(LED, OUTPUT); //LED initialisieren
  digitalWrite(LED, HIGH); //LED einschalten
  
  initializeButtons(); //Taster initialisieren
  initializePID(); //PID-Regler initialisieren
  initializeMotor(); //Motoren initialisieren
  initializeIMU(); //IMU initialisieren
  lastTime = millis(); //Timer starten
}

void loop() {
  checkButtons();
  calculateAngle(); //Kippwinkel aus der IMU bestimmen
  calculateWay(); //Weg aus Drehencodern berechnen
  printInfo();
  if(wayControlEnabled == true){
    wayControl.Compute(); //Stellgröße berechnen
  }
  angleControl.Compute(); //Stellgröße berechnen
  if(enabled == true){
    moveMotor(motorPWM); //Motoren bewegen
  }
  checkSerial();
}
