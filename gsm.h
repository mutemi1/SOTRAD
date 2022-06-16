////////////////GSM/////////////////
void Clear_Buffer() {
  Serial_Data_Received();
  int i;
  for (i = 0; i < 256; i++) {
    inputString[i] = 0x00;

  }
  buffer_pos = 0;
}


void Serial_Data_Received() {

  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    inputString[buffer_pos] = inChar;
    buffer_pos++;
  }
  ////Serial.println("**********************DATA IN BUFFER*****************");
  ////Serial.println(inputString);
  ////Serial.println("**********************DATA IN BUFFER*****************");
}




