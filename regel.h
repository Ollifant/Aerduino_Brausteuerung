class TRegel
{
  public:
    TRegel(byte relaisPin, byte ledRelaisPin, byte max31865Pin);
    void setTemp(int delta);
    void setTimer(long delta);
    int tempSteuerung();
    void initMAX31865();
  private:
    byte _relaisPin;                // Pin des Relais
    byte _ledRelaisPin;             // Pin der LED, die den Zustand des Relais anzeigt
    byte _max31865Pin;              // Pin des MAX31865, der die Temperatur misst
    int _tSoll;                     // Soll-Temperatur
    int _tHysterese;                // Hysterese = max. Abweichung von Soll-Temperatur
    int _tIst;                      // Ist-Temperatur
    long _lastDebounceTime;         // the last time the relais was toggled
    long _debounceDelay;            // the debounce time; increase if the relais flickers
    boolean _relaisState;           // false=Relais aus; true=Relais an
    long _tDauer;                   // Dauer der Solltemperatur in Millisekunden
    long _tStart;                   // Startzeitpunkt wann die Soll-Temperatur zum 1. Mal erreicht wurde
    boolean _newTime;               // true= neue Zeitzählung; false=Zeit läuft weiter
    long _newTimer;                 // Zeitpunkt, ab dem die neue Zeit zählt
    boolean _alarm;                 // Alarm wird ausgelöst, wenn Timer abgelaufen
    String _1Zeile;                 // Text, der in der ersten Zeile des Displays angezeigt wird
    String _2Zeile;                 // Text, der in im Display zur Timereinstellung angezeigt wird
    boolean _MAX31865Flag;          // Flag, das anzeigt, dass ein MAX31865 angeschlossen ist
    double _tempKorrektur;          // Korrekturwert für die Temperaturmessung

    int readMAX31865Temp();
    void switchRelais(boolean relaisState);
    void setValues();
    void showValues();
    void showAlarm();               // Verbindung zu Sensor verloren
    void showTimer(long sekunden);  // Anzeige des Timers in Minuten
};

TRegel::TRegel(byte relaisPin, byte ledRelaisPin, byte max31865Pin){    // Konstruktor
  // die Pins in lokalen Variablen speichern
  _relaisPin = relaisPin;
  _ledRelaisPin = ledRelaisPin;
  _max31865Pin = max31865Pin;
  // Feste Werte für Variablen definieren
  _tHysterese = 2;        // 0,2 Grad Hysterese
  _lastDebounceTime = 0;   
  _debounceDelay = 1000;  // min. 1000ms zwischen zwei Schaltvorgängen
  _newTime = true;        // neue Zeitmessung
  _newTimer = millis();
  _tDauer = 0;
  _alarm = false;
  _tempKorrektur = -7;
  // Relais- & LED Pins als Ausgang definieren
  pinMode(_relaisPin, OUTPUT);
  pinMode(_ledRelaisPin, OUTPUT);
  // Relais & LED ausschalten
  digitalWrite(_relaisPin, LOW);
  digitalWrite(_ledRelaisPin, LOW);
  _relaisState = false;
  // Text der Anzeigen definieren
  _1Zeile = "Temperatursteuerung";
  _2Zeile = "Zeit eingeben";
  
  // Temperatur initialisieren
  _tSoll = 300;
  _tIst = 1000;         //bei 100 Grad wird das Relais wahrscheinlich nicht eingeschaltet
}

void TRegel::setTemp(int delta){                // Definition der Solltemperatur
  _tSoll = _tSoll+delta;                        // Solltemperatur speichern
  
  if((delta >= 10) || (delta <= -10)){
    // Temperatur hat sich signifikant geändert - Zeitzählung neu starten
    if (_tSoll > _tIst){
      _newTime = true;
      logging("Neue Solltemperatur", _newTime);
    }
  }   
  
  if((delta == 10) ||(delta == -10)) {
    // letzte Stelle des Zählers auf Null setzen wenn in 10er Schritten gezählt wird
    // Integer hat keine Nachkommastellen
    _tSoll = _tSoll/10;
    _tSoll = _tSoll*10;
  }
  
  if (_tSoll < 0){
    // es darf keine Werte kleiner Null geben
    _tSoll = 0;
  }
  
  if (_tSoll > 1000){
    // es darf keine Werte größer 1000 = 100 Grad geben
    _tSoll = 1000;
  }
  // Solltemperatur loggen
  // logging("Solltemperatur", _tSoll);
}

void TRegel::setTimer(long delta){                // Setzen des Timers - Delta in Minuten
  int _sekunden;
  long _tmp;
  long _tmp2;
  
  if (delta > 0L){
    _alarm = true;                                // Timer wird gesetzt & Alarm eingeschaltet 
    logging("Alarm", _alarm);
  }
  
  delta = delta * 60 * 1000;                      // Delta in Millisekunden umrechnen
  
  if(_newTime == true){                           // Timer ändern, da Zeitzählung noch nicht gestartet
    _tDauer = _tDauer + delta;                    // Dauer setzen
  
    if (_tDauer < 0){
     _tDauer = 0;                                 // Zeit kann nicht negativ werden
    } 
  
    _sekunden = _tDauer / 1000;                   // Sekunden berechnen
    showTimer(_sekunden);                         // Timer anzeigen
    }
  else {                                          // Timer läuft oder ist bereits abgelaufen
    _tmp = _newTimer + delta;                     // Neue Endzeit berechnen
    _tmp2 = _tmp - millis();
    _sekunden = _tmp2 / 1000;                     // Restliche Sekunden berechnen

    if (_tmp2 <= 0){                              // neuer Timer bereits abgelaufen
      _newTimer = millis();                       // Timer auf aktuelle Zeit stellen
      showTimer(0);                               // 0 Minuten anzeigen
    }
    else {
      _newTimer = _tmp;
      showTimer(_sekunden);                       // Restliche Minuten anzeigen
      } 
  }
}

void TRegel::showTimer(long sekunden){               // Anzeigen des Timers
  int minuten;
  int restSekunden;
  
  lcd.clear(); 
  lcd.setCursor(3, 0);
  lcd.print(_2Zeile);
  
  minuten = sekunden / 60;
  restSekunden = sekunden % 60;
 
  // Timer anzeigen
  lcd.setCursor(1, 2);
  lcd.print("Timer: ");
  if (minuten < 100){
    // Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  if (minuten < 10){
    // zweites Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  lcd.print(minuten);
  lcd.print(":");
  if (restSekunden < 10){
    // 0 einfügen, damit die Anzeige linksbündig ist
    lcd.print("0");
  }
  lcd.print(restSekunden);
}

void TRegel::switchRelais(boolean relaisState){
  if (relaisState == true){
    // Relais & LED einschalten
    digitalWrite(_relaisPin, HIGH);
    digitalWrite(_ledRelaisPin, HIGH);
  }
  else{
    // Relais & LED ausschalten
    digitalWrite(_relaisPin, LOW);
    digitalWrite(_ledRelaisPin, LOW);
  }
  //Relais-Status speichern
  _relaisState = relaisState;
  // Relais-Status des Relais loggen
  // logging("Relais", _relaisState);
}

int TRegel::tempSteuerung(){                    // Steuerung der Temperatur, Rückgabewert ist die aktuelle Temperatur
  int i; 
  
  // MAX318565 Temperatur auslesen
  _tIst = readMAX31865Temp();
  if (_tIst <= 0){
    // Verbindung zu Sensor verloren
    // noch einmal versuchen
    _tIst = readMAX31865Temp();
    if (_tIst <= 0){
      showAlarm();
      return(_tIst);
    }
   }
  
  // Werte auf dem Display anzeigen
  showValues();

  // Prüfen, ob das minimale Delay abgelaufen ist
  if ((millis() - _lastDebounceTime) > _debounceDelay){
    // Prüfen, ob Soll-Temperatur erreicht ist
    if(_tIst >= _tSoll){
      // Relais ausschalten
      switchRelais(false);
      _lastDebounceTime = millis();          // Schaltzeitpunkt speichern
      if(_newTime == true){
        // neue Temperatur zum ersten Mal erreicht - Timer neu starten
        _newTime = false;
        // logging("Start Timer", _newTime);
        _newTimer = millis() + _tDauer;      // Endzeitpunkt speichern
        _tDauer = 0;                         // Dauer löschen
        lautSprecher(500);                   // Aufmerksamkeitston
      }
    }
    else{                                      
      // Prüfen, ob Hysterese-Temperatur unterschritten ist    
      if (_tIst < (_tSoll-_tHysterese)){
        // Relais einschalten
        switchRelais(true);
        _lastDebounceTime = millis();          // Schaltzeitpunkt speichern
      }
    }
  }
  
  if(_alarm == true){
    logging("Alarm", _alarm);
    if (_newTime == false){
      logging("_newTime", _newTime);
      if (_newTimer < millis()){
        logging("Restzeit", (_newTimer - millis()));
        // Timer abgelaufen
        for (i=1; i<5; i++){
         lautSprecher(200);                   // Aufmerksamkeitston
         delay(200);
        }
        _alarm = false;
      }
    }
  }
  return(_tIst);
}

void TRegel::initMAX31865(){
  digitalWrite(MAX31865Pin, LOW);  // take the chip select low to select the device:
  SPI.transfer(0x80);              // Send config register location to chip
                                   // 0x8x to specify 'write register value' 
                                   // 0xx0 to specify 'configuration register'
  SPI.transfer(0xC2);              // Write config to IC
                                   // bit 7: Vbias -> 1 (ON)
                                   // bit 6: conversion mode -> 1 (AUTO)
                                   // bit 5: 1-shot -> 0 (off)
                                   // bit 4: 3-wire select -> 0 (2/4 wire config)
                                   // bit 3-2: fault detection cycle -> 0 (none)
                                   // bit 1: fault status clear -> 1 (clear any fault)
                                   // bit 0: 50/60 Hz filter select -> 0 (60 Hz)
  digitalWrite(MAX31865Pin, HIGH); // take the chip select high to de-select, finish config write
  delay(100);
}
  
int TRegel::readMAX31865Temp(){                         // Liest den Wert des MAX31865 und berechnet die Temperatur
  unsigned char reg[8];          // array for all the 8 registers
  unsigned int i;
  unsigned int RTDdata;          // 16 bit value of RTD MSB & RTD LSB, reg[1] & reg[2]
  unsigned int ADCcode;          // 15 bit value of ADC, RTDdata >> 1, 0th bit of RTDdata is RTD connection fault detection bit
  double R;                      // actual resistance of PT100 sensor
  double T;                      // Temperatur of PT100 sensor
  // Werte für Callendar-Van Dusen equation I.
  double a = 3.9083E-03;
  double b = -5.7750E-07;
  signed long R0=100;
  
  digitalWrite(MAX31865Pin, LOW);
  delay(10);
  // Write command telling IC that we want to 'read' and start at register 0
  SPI.transfer(0);                                     // plan to start read at the config register
  // read registers in order:
	// configuration
	// RTD MSBs
	// RTD LSBs
	// High Fault Threshold MSB
	// High Fault Threshold LSB
	// Low Fault Threshold MSB
	// Low Fault Threshold LSB
	// Fault Status
  for (i=0; i<8; i++) reg[i]=SPI.transfer(0);          // read all the 8 registers
  delay(10);
  digitalWrite(MAX31865Pin, HIGH);                     // set CS high to finish read

  RTDdata = reg[1] << 8;                               // shift data 8 bits left
  RTDdata = RTDdata | reg[2];
  //Serial.print("RTD data: 0x");
  //Serial.println(RTDdata,HEX);
  
  if (RTDdata & 1) {
    //Serial.println("PT100 sensor connenction fault!");
    //Serial.println();
    T = 0;
    initMAX31865();
  }  
  else{ 
    //Serial.print("ADC code (decimal): ");
    ADCcode=RTDdata>>1;
    //Serial.println(ADCcode,DEC);

    //Serial.print("Resistance: ");
    R = (double)ADCcode*400/32768;
    //Serial.print(R);
    //Serial.println(" Ohms");
    
    //Serial.print("Temperature: ");
    T = (-R0*a+sqrt(R0*R0*a*a-4*R0*b*(R0-R)))/(2*R0*b); // Umrechnen des Widerstands in Temperatur nach Callendar-Van Dusen equation I.
    //Serial.print(T);
    //Serial.println(" deg. C");
    //Serial.println();
    
    T = T*10;               //Temperatur in Zehntel Grad
    T = T + _tempKorrektur; //gemessene Temperatur korregieren
  }
  
  return(T);
}

void TRegel::showValues(){      // Zeigt die Temperaturwerte auf dem Display an
  int _grad;
  int _zehntel;
  long _restzeit;
  
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print(_1Zeile);
 
  // Gradzahl in Ganzzahl und Nachkommastelle trennen
  _grad=_tSoll/10;
  _zehntel=_tSoll-(_grad*10);
  
  // Gradzahl anzeigen
  lcd.setCursor(4, 1);
  lcd.print("Soll: ");
  if (_grad < 100){
    // Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  if (_grad < 10){
    // zweites Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  lcd.print(_grad);
  lcd.print(",");
  lcd.print(_zehntel);
  
  // Gradzahl in Ganzzahl und Nachkommastelle trennen
  _grad=_tIst/10;
  _zehntel=_tIst-(_grad*10);
  
  // Gradzahl anzeigen
  lcd.setCursor(4, 2);
  lcd.print("Ist : ");
  if (_grad < 100){
    // Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  if (_grad < 10){
    // zweites Leerzeichen einfügen, damit die Anzeige rechtsbündig ist
    lcd.print(" ");
  }
  lcd.print(_grad);
  lcd.print(",");
  lcd.print(_zehntel);
  
  // Zeit anzeigen
  _restzeit = _newTimer - millis();
  lcd.setCursor(4, 3);
  lcd.print("Zeit:  ");
  if(_newTime == true){
    // Timer anzeigen, da Zeitzählung noch nicht gestartet
    displayTime(_tDauer);
    }
  else{
    if (_restzeit <=0){
      // keine Zeit anzeigen
      displayTime(0);
    }
    else{
      // Restzeit anzeigen
      displayTime(_restzeit);
      }
  }
}

void TRegel::showAlarm(){      // Zeigt an, dass die Verbindung zum Sensor verloren gegangen ist
  // Relais ausschalten
  switchRelais(false);
  // Alarm anzeigen
  lcd.clear(); 
  lcd.setCursor(3, 0);
  lcd.print("!!!  ALARM  !!!");
  lcd.setCursor(3, 2);
  lcd.print("Temperatursensor");
  lcd.setCursor(3, 3);
  lcd.print("fehlt !!!");
  // Alarm senden
  lautSprecher(500);

}
