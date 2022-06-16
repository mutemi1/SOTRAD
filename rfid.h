/////////////////////////RFID///////////////////////////
void RFID_Check() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  else {                                            ////If a card is detected immediately buzz for a short time
    digitalWrite(buzz, HIGH);
    delay(400);
    digitalWrite(buzz, LOW);

  }


  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    ////Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    ////Serial.print(mfrc522.uid.uidByte[i], HEX);
    // content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  char_arr = &content[0];                  ///convert string to character array
  ////Serial.println();
  ////Serial.println(char_arr);
  Get_Time();                               ///Read RTC
  strcat(text, Device_ID_buffer);          ////Formatting
  strcat(text, ",");
  strcat(text, "R");
  strcat(text, ",");
  strcat(text, char_arr);
  strcat(text, ",");
  strcat(text, Exact_time_data);
  strcat(text, "\n");
  strcpy(SD_Card_Data, text);
  SD_write();                                ///Write to SD CARD

  digitalWrite(buzz, HIGH);                  //Buzzer goes off after sending data
  delay(1000);
  digitalWrite(buzz, LOW);
  delay(2000);
  strcpy(text, "");

}

void Buzzer() {                /////For the starting sequence
  digitalWrite(buzz, HIGH);
  delay(10);
  digitalWrite(buzz, LOW);
  delay(1200);

}
