/*
The circuit:
 * pushbutton attached from pin x to +5V
 * 10K resistor attached from pin x to ground
 http://www.arduino.cc/en/Tutorial/Debounce
 */
 
#include "Arduino.h"
#include "logging.h"
#define AnzPunkte 3

class DrehSchalter
{
  public:
    DrehSchalter(int drehPin);
    int readDrehSchalter();
  private:
    int offsetDrehSchalter();
    int _drehPin;                      // Pin, an dem der DrehSchalter angeschlossen ist
    unsigned long _lastSwitchTime;     // Letzter Zeitpunkt an dem der DrehSchalter geschaltet hat 
    unsigned long _switchDelay;        // Zeit zum unterscheiden von schnellen und langsamen Schalten
    unsigned long _switchTime[AnzPunkte];      // Speicher der letzten Schaltzeitpunkte
    int _lastButtonState;              // Letzter Status des DrehSchalters
    int _newButtonState;               // Aktueller Status des Drehschalters
    int _offset;                       // Wert, um den der Counter hoch oder runter gezählt wird
};

DrehSchalter::DrehSchalter(int drehPin){    //Konstruktor
  // den Pin in lokaler Variablen speichern
  _drehPin = drehPin;
  // States initialisieren
  _lastButtonState = 1;
  _newButtonState = 1;
  _lastSwitchTime = millis();  //auf aktuelle Zeit setzen
  _switchDelay = 1000;
  _offset = 1;
  // Speicher der Schaltzeitpunkte löschen
  for(int i=0; i<AnzPunkte; i++){
    _switchTime[i]==0;
  }
}

int DrehSchalter::readDrehSchalter(){
  int _readingButton = 0;
  int _delta=0;
  
  // read the value from the sensor
  _readingButton = analogRead(_drehPin); 
  // Status des Drehschalters loggen
  // logging("Drehschalter", _readingButton);
  
  // Spannungswert konvertieren
  _newButtonState = map(_readingButton, 0, 1024, 0, 4);
  // MapStatus des Drehschalters loggen
  // logging("Map", _newButtonState);
  
  if (_newButtonState == _lastButtonState){
    // Schalter wurde nicht gedreht
    return(0);
  }
  
  // Wenn der Drehschalter schnell gedreht wird, dann soll in 10er Schritten gezählt werden
  _offset = offsetDrehSchalter();
  
  switch(_lastButtonState){
    case 1:
      if (_newButtonState == 3){
        // vorwärts gedreht - Zähler inkrementieren
        _delta = _offset;
      }
      else{
        // rückwärts gedreht - Zähler dekrementieren
        _delta = 0 - _offset;
      }
      break;
    case 2:
      if (_newButtonState == 1){
        // vorwärts gedreht - Zähler inkrementieren
        _delta = _offset;
      }
      else{
        // rückwärts gedreht - Zähler dekrementieren
        _delta = 0 - _offset;
      }
      break;
    case 3:
      if (_newButtonState == 1){
        // rückwärts gedreht - Zähler dekrementieren
        _delta = 0 - _offset;
      }
      else{
        // vorwärts gedreht - Zähler inkrementieren
        _delta = _offset;
      }
      break;
    default:
      // Fehler, kann nicht sein
      logging("Drehschalter Status falsch:", _lastButtonState);
      break;
  }
  
  // Status für nächsten Durchlauf speichern
  _lastButtonState = _newButtonState;
  //Schaltzeit auf aktuelle Zeit setzen
  _lastSwitchTime = millis();  
  // Counter loggen
  logging("Delta", _delta);
  return(_delta);
}

int DrehSchalter::offsetDrehSchalter(){
  // Verschieben der bisher gepeicherten Werte
  for(int i=AnzPunkte-2; i>=0; i--){
    _switchTime[i+1]= _switchTime[i];
  }
  // Speichern des aktuellen Wertes
  _switchTime[0] = millis();
  
  // logging("Switch Time 1", _switchTime[0]);
  // logging("Switch Time n", _switchTime[AnzPunkte-1]);
  
  // Wenn der Schalter langsam gedreht wird, soll in kleinen Schritten gezählt werden - sonst in großen
  if(_switchTime[0] > (_switchTime[AnzPunkte-1] + _switchDelay)){
    return(1);
  }
  else{
    return (10);
  }
}

class Taster
{
  public:
    Taster(int tasterPin);
    int readTaster();
  private:
    int _tasterPin;          // Pin, an dem der Taster angeschlossen ist
    int _lastDebounceTime;   // Letzter Zeitpunkt an dem der Taster geschaltet hat 
    int _debounceDelay;      // Zeit zum stabilisieren der Spannung
    int _lastButtonState;    // Letzter Status des Tasters
    int _newButtonState;     // Aktueller Status des Tasters
};

Taster::Taster(int tasterPin){    //Konstruktor
  // die Pins in lokalen Variablen speichern
  _tasterPin = tasterPin;
  // Feste Werte für Variablen definieren
  _lastDebounceTime = 0;   
  _debounceDelay = 50; 
  // States initialisieren
  _lastButtonState = LOW;
  _newButtonState = LOW;
}

int Taster::readTaster(){    //Prüft, ob der Taster gedrückt wurde: Rückgabewert ist der Status
  // Status des Buttons lesen
  if (analogRead(_tasterPin) < 900) {
    _newButtonState = true;      // Taster gedrückt
  }
  else {
    _newButtonState = false;     // Taster NICHT gedrückt
  }
  
  // Prüfen, ob sich der Status geändert hat
  if (_newButtonState != _lastButtonState) {
    // Abwarten, bis die Karrenzzeit abgelaufen ist
    if ((millis() - _lastDebounceTime) > _debounceDelay) {
      // Neuen Status setzen
      _lastButtonState = _newButtonState;
      _lastDebounceTime = millis();
      // Status des Tasters loggen
      //logging("Taster", _lastButtonState);
    }
  }
  else{
    _lastDebounceTime = millis();
  }
  // Status zurückgeben
  return(_lastButtonState);
}
