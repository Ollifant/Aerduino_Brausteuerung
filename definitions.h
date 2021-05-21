#define ToneDuration 200      // Dauer des Tons

/* -------------------------------------------------------------
 definiert das PIN Layout des Arduino
 --------------------------------------------------------------*/
#define DrehPin 2             // analoger Input Pin für Drehschalter
#define TasterPin 0           // analoger Input für den Taster

#define RelaisPIN 3           // digitaler Output zur Ansteuerung des Relais
#define LEDPin 1              // digitaler Output zur Ansteuerung der LED
#define MAX31865Pin 10        // digitaler Output zu Ansteuerung des MAX31865, der den Wert des PT100 liest       
#define SpeakerPin 2          // digitaler Output zur Ansteuerung des Lautsprechers

/* -------------------------------------------------------------
 The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 http://arduino.cc/en/Tutorial/LiquidCrystal
 --------------------------------------------------------------*/
#define LCD_RS 8
#define LCD_Enable 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7


