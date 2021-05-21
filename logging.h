void logging(String key, int value){
  Serial.print(millis());
  Serial.print("\t");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(value);
}

void loggingText(String key){
  Serial.print(millis());
  Serial.print("\t");
  Serial.print("Display: ");
  Serial.println(key);
}

void lautSprecher(int dauer){
  /// Lautsprecher für Anz Millisekunden ansteuern
  digitalWrite(SpeakerPin, HIGH);
  delay(dauer);
  digitalWrite(SpeakerPin, LOW);
  
}
