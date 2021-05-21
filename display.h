LiquidCrystal lcd(LCD_RS, LCD_Enable, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


void initDisplay(){     
  // Initialisiert das Display und legt die Dimensionen fest lcd.begin(cols, rows)
  lcd.begin(20, 4);  
  // Löscht den Inhalt des Displays  
  lcd.clear();  
}

void displayTime(unsigned long time){    // Zeigt die Zeit an einer bestimmten Stelle auf dem Display an
    int seconds = (time/1000) % 60;
    int minutes = (time /60000) % 60;
    int hours = time /3600000;

    // Schreibe Stunden wenn größer 0
    if(hours > 0) {
      if(hours < 10) {
        lcd.print("0");
      }
      lcd.print(hours);
      lcd.print(":");
    }
    
    // Schreibe Minuten
    if (minutes < 10) {
      lcd.print("0");
     }
    lcd.print(minutes);  
    lcd.print(":");
    
    // Schreibe Sekunden
    if (seconds < 10){
      lcd.print("0");
     }
    lcd.print(seconds);  
}

void displayText(int zeile, int spalte, char text[]){    // Zeigt den Text an einer bestimmten Stelle auf dem Display an
  // Cusor plazieren
  lcd.setCursor(spalte, zeile);
  // Text ausgeben
  lcd.print(text);
}
