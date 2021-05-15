#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "RTClib.h"

int Device_ID = 2;
char Device_ID_buffer[5];

unsigned long int energy = 0;
char energy_buffer[50];
int pump_status = 0;
char pump_status_buffer[5];
int buzz = 7; //pin 7 for the Buzzer

//////////////COUNTER///////////////

unsigned long int   counter_A = 0;
unsigned long int   counter_B = 0;
unsigned long EEPROM_aggregate = 0;
char EEPROM_aggregate_buffer[20];
char  aggregate_buffer[50];
unsigned long int aggregate = 0;
int store_data_time;
unsigned long int  *p;
bool flip = false;          // Flag to alternate the two counters
const byte interruptPin = 2;
int i;
int delay_time = 10;      //Determines how many seconds the for loop in the main loop will run. 10 for 10 seconds
int  alternate_time = 0;  //Incremented every 10 secs
int  alternate_time_reached = 180;  //alternate counter after 30 mins(30x60=1800)/10=180

//////////////GSM/////////////////////
char inputString[256];         // to hold incoming data
char text[256];               //to hold data to be sent via the send_sms() function
int pending_sms = 0;
int buffer_pos;

int today_message_sent;
int ready_to_send;

/////////////////RFID//////////////////////////////


#define SS_PIN 53
#define RST_PIN 30
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
char* char_arr;
char rfid_buffer[10] = "00000000";

/////////////////RTC/////////////////////

RTC_DS1307 rtc;                   // Create RTC instance.
char  year_buffer[10];
char  month_buffer[10];
char  day_buffer[10];
char  hour_buffer[10];
char  min_buffer[10];
char  sec_buffer[10];
char  SMS_time_data[50];
char  Exact_time_data[50];

int credit_already_checked = 0;
int credit_check_ready = 0;


int evening_send_time = 18;
int morning_send_time = 6;
int midnight = 00;

/////////////////SD_CARD////////////////////////

File myFile;              // Create MYFILE instance.
char  SD_Card_Data[50];
char SD_buffer [50];


/////////////////EEPROM////////////////////////
int addr = 0;
int a;
int ready_to_send_addr = 50;          //Specific EEPROM addresses for the two flags
int today_message_sent_addr = 51;

int time_to_write_to_EEPROM = 2;   //wite to the EEPROM after every hour(1x60x60)/10=30
char EEPROM_buffer [100];
char EEPROM_counter[50];

////////////////SATELLITE//////////////

int diff;
char  satellite_buffer[50];
char  pre_satellite_buffer[50];
int time_to_send = 0;
int satellite_send_time = 0;
int satellite_time_reached = 30; ///Every 5 mins (5x60)/10=30

int sec = 0;


/////////////////ERROR DETECTION///////////////
int temp, total = 0;
int xor_number = 0;
char xor_buffer[5];
char total_buffer[5];
char c,z;

///////////////////////////////////////////////
void setup() {

  Serial.begin(4800);
  Serial1.begin(4800);
  //Serial.print("DEVICE ID: ");
  ////Serial.println(Device_ID);

  itoa(energy, energy_buffer, 10);
  itoa(pump_status, pump_status_buffer, 10);
  itoa(Device_ID, Device_ID_buffer, 10);

  SPI.begin();           // Initialize MFRC522
  mfrc522.PCD_Init();   // Initialize MFRC522

  pinMode(buzz, OUTPUT);   //BUZZER
  digitalWrite(buzz, LOW); //BUZZER OFF
  buffer_pos = 0;

  p = &counter_A;
  SD_begin();          //Initialize SD


  if (! rtc.begin()) {
    ////Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    ////Serial.println("RTC lost power, lets set the time!");

    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), increment, HIGH);

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


//////////////COUNTER FUNCTIONS///////
void increment() {
  ////Serial.println("Pointer Increment!");
  (*p)++;
}
void alternate() {
  //Serial.print("Aggregate Value Before: ");
  ////Serial.println(aggregate);
  if (flip == false) {
    aggregate = aggregate + counter_A;   //increment aggregate
    itoa(aggregate, aggregate_buffer, 10);
    p = &counter_B;
    flip = true;
    counter_A = 0;
  }

  else if (flip == true) {
    aggregate = aggregate + counter_B;    //increment aggregate
    itoa(aggregate, aggregate_buffer, 10);
    p = &counter_A;
    flip = false;
    counter_B = 0;
  }

  //Serial.print("Aggregate Value After: ");
  ////Serial.println(aggregate);
}

//////////////ERROR ENCODING////////////////
void error_encode() {                     // Gets the sum and xor of each bytes of the data. The last digit of the sum is first appended to the data.
                                          //The XoR result is converted to Hex Format then appended to the data
  total = 0;
  xor_number = 0;

  for (int j = 0; j < 32; j++) {
    char ch[2];
    ch[0] = pre_satellite_buffer[j];
    ch[1] = 0x00;
    temp = strtol(ch, NULL, 16);
    total += temp;
    xor_number ^= temp;
  }

  //Serial.print("TOTAL:");
  ////Serial.println(total);
  //Serial.print("XOR:");
  ////Serial.println(xor_number);

  itoa(xor_number, xor_buffer, 16);
  
  int n=0;
  while(xor_buffer[n]!='\0'){
    z=toupper(xor_buffer[n]);
    n++;
    
   }
   
  itoa(total, total_buffer, 10);

  int k, check = 0;
  while (total_buffer[k] != '\0') {
    check++;
    k++;
  }

  c = total_buffer[check - 1];
  strncat(pre_satellite_buffer, &c, 1);
  strncat(pre_satellite_buffer, &z, 1);
  
 

}
//////////////SATELLITE/////////////
void send_to_satellite() {                   //Sends Data to Satellite 

  strcpy(satellite_buffer, "AT+TX=");
  get_all_data();
  error_encode();

  ///////////////////////////////////////////////////////////
  strcat(satellite_buffer, pre_satellite_buffer);
  strcat(satellite_buffer, "\r");
  //Serial.print(satellite_buffer);
  Serial1.print(satellite_buffer);  
  Clear_Satellite_Buffer();

}

void Clear_Satellite_Buffer() {     

  int i;
  for (i = 0; i < 32; i++) {
    pre_satellite_buffer[i] = 0x00;

  }

}

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
/////////////////////////RTC///////////////////////
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
