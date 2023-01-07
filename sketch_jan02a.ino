#include "DHT.h"

//Status codes
int localStatus = -1;
int lockState = -1;
int startState = -1;
int isKeyless = 0;
int temperature;
int ledR = 0;
int ledG = 0;
int ledB = 0;

//Setup Codes
#define DHTTYPE DHT22

//Global Temps
int tempStart;
int handoffTime
int holdStatus;
int doorStatus;
float humi;
float tempF;
int ledFlash;

//PIN Locations
int tackPIN = 22;
int lockPIN = 23;
int unlockPIN = 24;
int accPIN = 25;
int alivePIN = 26;
int starterPIN = 27;
int killswitchOverride = 28;
int ledRPIN = 29;
int ledGPIN = 30;
int ledBPIN = 31;
int temperaturePIN = 32;
int resetPIN = 33;

//Timeouts and Preferences
int keylessTimeout = 300;
int startingTimeout = 8;
int startedTimeout = 600;



void setup() {
  Serial.begin(9600);
  Serial.println("SETUP MODE");

  Serial.println("Setting up pin modes.");
  pinMode(tackPIN, INPUT);
  pinMode(temperaturePIN, INPUT);
  pinMode(lockPIN, OUTPUT);
  pinMode(unlockPIN, OUTPUT);
  pinMode(accPIN, OUTPUT);
  pinMode(alivePIN, OUTPUT);
  pinMode(starterPIN, OUTPUT);
  pinMode(killswitchOverride, OUTPUT);
  pinMode(ledRPIN, OUTPUT);
  pinMode(ledGPIN, OUTPUT);
  pinMode(ledBPIN, OUTPUT);
  pinMode(resetPIN, OUTPUT);
  digitalWrite(resetPIN, LOW);
  Serial.println("Checking current status. (Setup Notice)");
  if (digitalRead(tackPIN) == HIGH) {
    localStatus = 1;
    Serial.println("Car is already on! (Setup Notice)");
  } else {
    localStatus = 0;
    Serial.println("Car is off! (Setup Notice)");
  }

  Serial.println("Setting up and checking temperature. (Setup Notice)");
  DHT dht(temperaturePIN, DHTTYPE);
  dht.begin();
  humi = dht.readHumidity();
  tempF = dht.readTemperature(true);
  Serial.println("Temperature setup reads -> humi = " + String(humi) + "% <> tempF = " + String(tempF) + "*F. (Setup Notice)");
  
  Serial.println("///EXITING SETUP MODE///");
}

void loop() {
  //localStaus Checks
  holdStatus = localStatus;
  if (digitalRead(tackPIN) == HIGH) { //Car is running (1)
    localStatus = 1;
    if (holdStatus != localStatus) {Serial.println("Car is now running. (From Key)");}
  } else if (localStatus != 2){ //Car is not running (0)
    localStatus = 0;
    if (holdStatus != localStatus) {Serial.println("Car is not longer running. (From Key)");}
  } else if (localStatus == 2){ //Car is starting from keyless (2)
    tempStart = tempStart - 1;
    if (tempStart == 0) {
      localStatus = 0;
      if (holdStatus != localStatus) {Serial.println("Car is failed to start. (From Keyless)");}
    } else {
      if (holdStatus != localStatus) {Serial.println("Car is trying to start. (From Keyless)");}
    }
  } else if (localStatus == 3) { //Keyless handoff mode (3)
    keylessTimeout = keylessTimeout - 1;
    if (keylessTimeout == 0) {
      localStatus = 0;
      if (holdStatus != localStatus) {Serial.println("Keyless handoff mode timed out. (From Keyless)");}
    } else {
      if (holdStatus != localStatus) {Serial.println("Keyless handoff enabled. (From Keyless)");}
    }
  } else if (localStatus == -1) { //Setup mode exit (-1)
    localStatus = 0;
  } else if (localStatus == -2) { //Restart Arduino (-2)
    digitalWrite(resetPIN, HIGH);
  }

  //Aux Checks (Door)
  if (doorStatus == 1) {
    digitalWrite(lockPIN, HIGH);
    doorStatus = 0;
    Serial.println("Locking car. (From Keyless)");
  } else if (doorStatus == 2) {
    digitalWrite(unlockPIN, HIGH);
    doorStatus = 0;
    Serial.println("Unlocking car. (From Keyless)");
  } else if (doorStatus == 0) {
    digitalWrite(lockPIN, LOW);
    digitalWrite(unlockPIN, LOW);
  }

  //Temperature Checks
  DHT dht(temperaturePIN, DHTTYPE);
  dht.begin();
  humi = dht.readHumidity();
  tempF = dht.readTemperature(true);

  //LED
  digitalWrite(ledR, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(ledB, LOW);
  if (localStatus == 0) { //Car is off.
      if (ledFlash == 0) {
        ledFlash == 1;
        digitalWrite(ledB, HIGH);
      } else {
        ledFlash == 0;
      }
  } else if (localStatus == 1) { //Car is running.
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    digitalWrite(ledB, LOW);
  } else if (localStatus == 2) { //Car is starting.
    digitalWrite(ledR, HIGH);
  } else if (localStatus == 3) { //Keyless handoff.
    if (ledFlash == 0) {
        ledFlash == 1;
        digitalWrite(ledG, HIGH);
      } else {
        ledFlash == 0;
      }
  } else if (localStatus == 4) { //Not used yet.

  }
}
