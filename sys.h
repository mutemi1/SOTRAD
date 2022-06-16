
void system_init(){

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
