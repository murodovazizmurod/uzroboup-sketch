// relesini isshlatgan joyinacha galdi
// muzika yozildi
// 25.09.2020

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define GAZ      220
#define GAZRELE  120
#define OFFTIME  300


SoftwareSerial mySerial(D6, D5);
unsigned long time1, time2, time3;
int  status       = 0;
int  gasValue     = 0;
int  mode1        = 0;
int  passlen      = 0;
int  signal       = 0;
char temp1, temp2;
String password   = "";
String ssid       = "";
String phone      = "";
String tmpSim     = "";
String atcommand  = "";
IPAddress accessPointIP     = IPAddress(192, 168, 1, 1);
WiFiServer server(80);
WiFiClient client;

int melody[] = {
  262, 196, 196, 220, 196, 0, 247, 262
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int reak1 = 0, reak2 = 0, reak3 = 0;
String msg11 = "";


void initialSim() {
  Serial.println( "SALOM" );
  mySerial.println("AT");
  updateSerial();
  delay(3000);
  mySerial.println("AT+CBC");
  updateSerial();
  delay(5000);
  mySerial.println("AT+CSQ");
  updateSerial();
    mySerial.println("AT+CSCS=GSM");
  updateSerial();
  delay(5000);
  signal = tmpSim.substring(14, 17).toInt();
  signal /= 6;
  Serial.println();
  Serial.println(signal);
  // mySerial.println("ATD+ +998937463664;");// tel getadi
  //  delay(40000);
  //  //  updateSerial();
  //  mySerial.println("AT+CNMI=1,2,0,0,0");
  //  updateSerial();

  mySerial.println("AT+CMGF=1");
  updateSerial();
  delay(5000);


  delay(3000);
  // jillichilik
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(D2, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(D2);
  }
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(D2, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(D2);
  }
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  EEPROM.begin(512);


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);


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
  initialSim();


  WiFi.disconnect();
  WiFi.softAPdisconnect(false);
  WiFi.mode(WIFI_STA);
  WiFi.softAPConfig(accessPointIP, accessPointIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  server.begin();


}
void loop() { 

  if (time3 + 100 < millis() && gasValue > GAZ) {
    time3 = millis();
    if (analogRead(A0) > 200) {
      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\""+phone+"\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print("Men Aqilli Gazadetektor xonada gaz sizib chiqishini aniqladim."); //text content
      updateSerial();
      mySerial.write(26);
    }
  }
  if (time2 + 50 < millis() ) {
    time2 = millis();
    gasValue = analogRead(A0);
    String request, header1;
    client = server.available();

    if (!client) {
      return ;
    }
    header1 = client.readStringUntil('\n') + "\r\n";
    header1 = client.readStringUntil('\n') + "\r\n";
    request = client.readStringUntil('\n') + "\r\n";
    request = request.substring(7);
    DynamicJsonDocument currentjsonDocument(1024);
    DeserializationError jsonError = deserializeJson(currentjsonDocument, request);
    if (!jsonError) {
      if (!digitalRead(LED_BUILTIN) && status != 1) {
        digitalWrite(LED_BUILTIN, HIGH );
      } else {
        digitalWrite(LED_BUILTIN,  LOW);// D4 ga ulanadi
      }
      serializeJson(currentjsonDocument, request);
      JsonObject joset = currentjsonDocument.as<JsonObject>();
      joset["mode"]           = mode1         = joset["mode"]       | mode1;
      client.flush();
      String s = "HTTP/1.1 200 OK\r\n";
      s += "Content-Type: application/json\r\n\r\n";
      s += "{\"gasValue\":" + String(gasValue)  + ",\"signal\":" + signal + ",\"second\":" + String(333) + "}\r\n";
      s += "\r\n\n";
      client.print(s);

      if (mode1 == 4) {
        joset["password"] = password      = joset["password"] | password;
        joset["ssid"] = ssid      = joset["ssid"] | ssid;
        joset["phone"] = phone      = joset["phone"] | phone;
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
      }
      if (mode1 == 5) {
        status = joset["status"];
        EEPROM.write(50,  status );
      }
      EEPROM.commit();
    }
  }
}
String updateSerial()
{
  int indexx = 0;
  char hh; tmpSim = "";
  delay(30);
  while (mySerial.available())
  {
    hh = mySerial.read();

    //    Serial.print(indexx++);
    //    Serial.print("--> ");
    Serial.print(hh);
    tmpSim += hh;

  }
  if (tmpSim.indexOf("+CMTI:") != -1) {
    Serial.println("!!!!!!!!!!!");
    tmpSim = tmpSim.substring(10, 3);
    Serial.println(tmpSim);
  }
  return tmpSim;
}
