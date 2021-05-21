#include <LiquidCrystal.h>
#include <SPI.h>
#include "definitions.h"
#include "taster.h"
#include "display.h"
#include "regel.h"

TRegel tRegel(RelaisPIN,LEDPin, MAX31865Pin);    // Initialisiert die Instanz mit den Nummern der Pins für Relais, LED und MAX31865
DrehSchalter drehSchalter(DrehPin);              // Initialisiert die Instanz mit der Nummer des Pins für den Drehschalter
Taster taster(TasterPin);                        // Initialisiert die Inztanz mit der Nummer des Pins für den Taster

void setup()
{
  // Serial.begin muss auskommentiert werden, da LED mit Pin D1 verbunden
  //Serial.begin(9600);
  // Initialisiert das LCD Display
  initDisplay();
  // Lautsprecher Pin initialisieren
  pinMode(SpeakerPin, OUTPUT);
  // SPI Bus mit MAX31865 initialisieren
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE3);
  pinMode(MAX31865Pin, OUTPUT);
  digitalWrite(MAX31865Pin, HIGH);
  delay(100);
  // MAX31865 mit PT100 initialisieren
  tRegel.initMAX31865();    // Prüfen, ob MAX31865 vorhanden & initialisieren
  // Drehschalter initialisieren
  drehSchalter.readDrehSchalter();
}

void loop()
{ 
  if (taster.readTaster() == true){     // Taster gedrückt?
    // Timer gemäß Drehschalter setzen
    tRegel.setTimer(drehSchalter.readDrehSchalter());
  }
  else{
     // Soll-Temperatur gemäß Drehschalter setzen
     tRegel.setTemp(drehSchalter.readDrehSchalter());
     // Temperatur regeln
     tRegel.tempSteuerung();
  }
  
  delay(50);
}
