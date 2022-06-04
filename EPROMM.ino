#include <EEPROM.h>

String password   = "";
String ssid       = "";
String phone      = "";
String tmpSim     = "";
String atcommand  = "";
int passlen=0; 
char temp1;
void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);


  password      = "12345678";
  ssid          = "Gaz Analiz";
  phone         = "+998904330422";
  passlen =  password.length();
  EEPROM.write(250,  passlen );
  EEPROM.write(249,  ssid.length());
  EEPROM.write(248,  phone.length());
  for (int i = 0; i < ssid.length(); i++) {
    temp1 = ssid[i];
    EEPROM.write(100 + i, temp1);
  }
  for (int i = 0; i < phone.length(); i++) {
    temp1 = phone[i];
    EEPROM.write(150 + i, temp1);
  }
  for (int i = 0; i < passlen; i++) {
    temp1 = password[i];
    EEPROM.write(251 + i, temp1);
  }
  EEPROM.commit();

  // for phone number
  passlen = EEPROM.read(248);
  for (int i = 0; i < passlen; i++) {
    temp1 = EEPROM.read(150 + i);
    phone += temp1;
  }

  // for ssid
  passlen = EEPROM.read(249);
  for (int i = 0; i < passlen; i++) {
    temp1 = EEPROM.read(100 + i);
    ssid += temp1;
  }

  // for password
  passlen = EEPROM.read(250);
  for (int i = 0; i < passlen; i++) {
    temp1 = EEPROM.read(251 + i);
    password += temp1;
  }
  //
  Serial.println( );
  Serial.println( password );
  Serial.println( ssid );
  Serial.println( phone );



}
int reak1 = 0, reak2 = 0, reak3 = 0;
void loop() {

}
