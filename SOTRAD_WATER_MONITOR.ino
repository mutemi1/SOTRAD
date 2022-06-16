#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "RTClib.h"
#include "sys.h"
#include "variables.h"
#include "eeprom.h"
#include "gsm.h"
#include "sat.h"
#include "rfid.h"
#include "rtc.h"
#include "sd.h"


///////////////////////////////////////////////
void setup() {

 system_init();

}

void loop() {

  DateTime now = rtc.now();            ///create RTC instance
  Get_Time();
  ////Serial.println();
  //Serial.print("Time:");
  ////Serial.println(Exact_time_data);

  Serial_Data_Received();

  for (i = 0; i < delay_time; i++) {
    
    Serial_Data_Received();
    delay(1000);
    //RFID_Check();
    //Clear_Buffer();
  }


  alternate_time++;
  satellite_send_time++;

  if (alternate_time == alternate_time_reached) {
    ////Serial.println("TIME TO ALTERNATE COUNTERS...............");
    alternate();
    store_aggregate_to_EEPROM();
    alternate_time = 0;
  }

  if (satellite_send_time == satellite_time_reached) {  ///5 mins have elapsed  reset counter

    time_to_send = 1;
    satellite_send_time = 0;

  }
 
  Clear_Buffer();
  
  if (time_to_send == 1 && (now.hour() >= 17 && now.hour() <= 23) ) { //////// Send Satellite data every 10 mins(10x60/10=60)   between 5am and 11am
    ////Serial.println("TIME TO SEND SATELLITE DATA!!!!!!!!");

    send_to_satellite();
    time_to_send = 0;

  }

}

