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


void Clear_Satellite_Buffer() {     

  int i;
  for (i = 0; i < 32; i++) {
    pre_satellite_buffer[i] = 0x00;

  }

}
