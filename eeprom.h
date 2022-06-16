//////////////EEPROM//////////////////

void store_aggregate_to_EEPROM() {

  ////Serial.println("TIME TO WRITE TO EEPROM/SD CARD........!!!!!!!!!.........");
  Read_Counter_Value_From_EEPROM();
  get_all_data();

  strcpy(SD_Card_Data, SD_buffer);
  SD_write();                                        ////Write to SD CARD


  ////Serial.println("WRITING TO EEPROM..............................");    
  for (int k = 0; k < 40; k++) {
    EEPROM.write(addr, pre_satellite_buffer[k]);                         ///Write to EEPROM
    delay(10);
    addr++;
  }

  addr = 0;

}

void Read_Counter_Value_From_EEPROM() {

  ////Serial.println("READING EEPROM COUNTER DATA..............");

  int x = 0;
  for (int k = 5; k < 13; k++) {

    EEPROM_counter[x] = EEPROM.read(k);
    x++;
  }

  EEPROM_aggregate = strtol (EEPROM_counter, NULL, 10);
  EEPROM_aggregate += aggregate;
  itoa(EEPROM_aggregate, EEPROM_aggregate_buffer, 10);

  //Serial.print("EEPROM_counter: ");
  ////Serial.println(EEPROM_counter);
  //Serial.print("EEPROM_aggregate: ");
  ////Serial.println(EEPROM_aggregate);

}
