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