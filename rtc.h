/////////////////////////RTC///////////////////////

void get_all_data() {                   ///Gets all the data and puts it in a two character arrays: Pre_satellite_buffer and SD_card_buffer

  DateTime now = rtc.now();            ///create RTC instance
  Get_Time();

  strcpy(pre_satellite_buffer, SMS_time_data);
  strcpy(SD_buffer, Exact_time_data);

  ////////////////////////////////////////////////
  diff = 8 - strlen(EEPROM_aggregate_buffer);     //Ensures Total Liters is 8 bytes long by appending zeros to the actual data 
  for (i = 0; i < diff; i++) {

    strcat(pre_satellite_buffer, "0");
    strcat(SD_buffer, "0");

  }
  strcat(pre_satellite_buffer, EEPROM_aggregate_buffer);
  strcat(SD_buffer, EEPROM_aggregate_buffer);
  ///////////////////////////////////////////////////

  diff = 7 - strlen(energy_buffer);               //Ensures Energy is 7 bytes long by appending zeros to the actual data 
  for (i = 0; i < diff; i++) {

    strcat(pre_satellite_buffer, "0");
    strcat(SD_buffer, "0");
  }

  strcat(pre_satellite_buffer, energy_buffer);
  strcat(SD_buffer, energy_buffer);

  ///////////////////////////////////////////////////////////

  strcat(pre_satellite_buffer, rfid_buffer);
  strcat(SD_buffer, rfid_buffer);
  strcat(SD_buffer, "\n");
  //Serial.print("pre_satellite_buffer: ");
  ////Serial.println(pre_satellite_buffer);
}


void Get_Time() {

  DateTime now = rtc.now();                  ///create RTC instance

  itoa(now.year() - 2000, year_buffer, 10);    
  strcpy(Exact_time_data, year_buffer);
  strcpy(SMS_time_data, year_buffer);

  itoa(now.month(), month_buffer, 16);
  strcat(Exact_time_data, month_buffer);
  strcat(SMS_time_data, month_buffer);

  if (now.day() < 10) {                 //Append a 0 before a single digit date e.g April 4 -> 04
    strcat(SMS_time_data, "0");
    strcat(Exact_time_data, "0");
  }

  itoa(now.day(), day_buffer, 10);
  strcat(Exact_time_data, day_buffer);
  strcat(SMS_time_data, day_buffer);

  if (now.hour() < 10) {
    strcat(Exact_time_data, "0");
  }

  itoa(now.hour(), hour_buffer, 10);
  strcat(Exact_time_data, hour_buffer);


  if (now.minute() < 10) {
    strcat(Exact_time_data, "0");
  }

  itoa(now.minute(), min_buffer, 10);
  strcat(Exact_time_data, min_buffer);

  if (now.second() < 10) {
    strcat(Exact_time_data, "0");
  }

  itoa(now.second(), sec_buffer, 10);
  strcat(Exact_time_data, sec_buffer);


}
