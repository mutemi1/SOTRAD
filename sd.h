/////////////////////////////////SD_CARD/////////////////////////////
void SD_begin() {
  //Serial.print("Initializing SD card...");

  if (!SD.begin(48)) {
    ////Serial.println("initialization failed!");
    while (1);
  }
  ////Serial.println("initialization done.");

}
void SD_write( ) {
  myFile = SD.open("sotrad5.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print("Writing to data.txt...");
    myFile.print(SD_Card_Data);
    // close the file:
    myFile.close();
    ////Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    ////Serial.println("error opening test.txt");
  }


}
void SD_read() {
  //////Serial.println("READING SD DATA............");
  // re-open the file for reading:
  myFile = SD.open("sotrad5.txt");
  if (myFile) {
    ////Serial.println("sotrad5.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    ////Serial.println("error opening test.txt");
  }

}