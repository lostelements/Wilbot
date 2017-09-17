/* MQTT IOT CONTROLLED ROOM BUDDY
   ESP2866 NodeMCU Based
   Max7219 display face with scrolling and Emoji's
   Continuous temperature monitoring From a DS1820  check room temperature and display face based on temperature, scroll temp every 5 minutes
    publishes room temp to 'roomtemp' MQTT service
    Subscribes to Messages to allow change of emoji and scrolling messages from mqtt
    Button to acknowledge message publish to mqtt
    Either pulsing neopixel or buzzer on message recipt

    starteded on auto config
    
     - DS18B20:

     + connect VCC (3.3V) to the appropriate DS18B20 pin (VDD)

     + connect GND to the appopriate DS18B20 pin (GND)

     + connect D4 to the DS18B20 data pin (DQ)

     + connect a 4.7K resistor between DQ and VCC.
 */

/*
 *  Includes see github lostelements for correct libraries
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //on Lostelemnts Git
#include <OneWire.h>
#include <DallasTemperature.h> //on LostElements Git
#include <SPI.h>
#include <LedMatrix.h>  //on Lostelements Git
#include <String.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>  

/*Variables */
#define SLEEP_DELAY_IN_SECONDS  30
#define ONE_WIRE_BUS            D4      // DS18B20 pin
#define BUFFER_SIZE 100 // for call back
#define buttonPin D1 //Switch pin
const char *ssid =  "BTHub5-SZR2";    // Your  ssid cannot be longer than 32 characters!
const char *pass =  "d4b2efef95";   // Your hub password
IPAddress server(192, 168, 1, 76); // IP address of Mosquitto server
//define set name of yor bot
String botname = "WILBOT";
//define mqtt message names
String roomtemp = "roomtemp\\" + botname;
String messages = "messages\\" + botname;
String dinner = "dinner\\" + botname;
String dick = "dick\\" + botname;
String acknowledge = "acknowledge\\" + botname;
int currentface = 1; 
#define NUMBER_OF_DEVICES 1 //number of Max7219 modules
#define CS_PIN 4 //data pin for Max7219
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
int x = 0;
WiFiClient wclient;
PubSubClient client(wclient, server);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
char temperatureString[6];
const unsigned long fiveMinutes = 5 * 60 * 1000UL;
static unsigned long lastSampleTime = 0 - fiveMinutes; // initialize such that a reading is due the first time through loop()
int buttonState = 0;

const byte face_font [][8] PROGMEM = {
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //Clear 0
 {0x00, 0x36, 0x56, 0x90, 0x90, 0x56, 0x36, 0x00}, //Widemouth 1
 {0x00, 0x36, 0x56, 0x50, 0x50, 0x56, 0x36, 0x00}, //Partclosed 2
 {0x00, 0x36, 0x36, 0x30, 0x30, 0x36, 0x36, 0x00},//Closed Mouth 3
 {0x00, 0x46, 0x26, 0x10, 0x10, 0x26, 0x46, 0x00}, //Sad 4
 {0x00, 0x16, 0x26, 0x40, 0x40, 0x26, 0x16, 0x00},//Happy 5 
 {0x00, 0x16, 0x26, 0x40, 0x40, 0x24, 0x14, 0x00}, //Wink 6
 {0x40, 0xa0, 0xbe, 0x41, 0xbe, 0xa0, 0x40, 0x00}, //NobLong 7
 {0x40, 0xa0, 0xb8, 0x44, 0xb8, 0xa0, 0x40, 0x00}, //nobmedium 8
 {0x40, 0xa0, 0xa0, 0x50, 0xa0, 0xa0, 0x40, 0x00}, //nobshort 9
 {0x00, 0x08, 0xac, 0x0e, 0xaf, 0x0e, 0xac, 0x00}, //Rain 10
 {0x81, 0x42, 0x18, 0x3c, 0xbd, 0x18, 0x42, 0x81}, //Sun 11
 {0x00, 0xf8, 0x64, 0xf6, 0x7e, 0xf6, 0x64, 0xf8} //Ghost1 12
};


void callback(const MQTT::Publish& pub) {
  // handle message arrived use strcmp on multiple subscriptions
   Serial.print(pub.topic());
  Serial.print(" => ");
  if (pub.has_stream()) {
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
    Serial.println("");
  } else
    Serial.println(pub.payload_string());
if (strcmp(pub.topic().c_str(),dinner.c_str())==0){
  
      animate_face(1);//Eating Animation
     
}
if (strcmp(pub.topic().c_str(),dick.c_str())==0){
      animate_face(4);//Dick Animation
 }
if (strcmp(pub.topic().c_str(),messages.c_str())== 0){
   ledMatrix.setText(pub.payload_string());
  while  (digitalRead(buttonPin) == LOW){
  ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(100);
    sendtemp();
   }
 }
}
void displayFace(int faceIndex)
{
  /* iterates over all 8 columns and sets their values*/
 ledMatrix.clear();
   ledMatrix.commit();
   //New Progmem Font trial
 for (int i=0; i < 8; i++){
      ledMatrix.sendByte(0,i+1,pgm_read_byte (&face_font [faceIndex] [i]));
    }
   delay(1000);
   sendtemp(); 
  
  

}
void animate_face(int facetype) {
    while  (digitalRead(buttonPin) == LOW){
      Serial.println(digitalRead(buttonPin));
      switch (facetype){
        case 1:  //Eating
           displayFace(1);
           displayFace(2);
           displayFace(3);
           displayFace(2);
           break;
        case 2: //winking
           displayFace(5);
           displayFace(6);
           break;
        case 3: //ghost
           break;    
        case 4: //Nob
           displayFace(9);
           displayFace(8);
           displayFace(7);
           displayFace(8);
           break;     
      }
    }
    Serial.print ("button Pressed");
    client.publish(acknowledge, "1");
     ledMatrix.clear();
   ledMatrix.commit();
   // Reset the Temperature scroll display interval
    lastSampleTime = millis();
    lastSampleTime -= fiveMinutes;
}
void setup_wifi() {
   //
  // new section for on demand setup i hope
  //
  if (digitalRead(buttonPin) == HIGH) { //held down on startup
   WiFiManager wifiManager; 
   if (!wifiManager.startConfigPortal("OnDemandAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    } else {
    if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  } 
  }
  Serial.println("connected...yeey :)");
  // end new section
  //

 /*  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }*/

}







void reconnect() {

  // Loop until we're reconnected  - change to same as basic, try one
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(botname)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
     /* Serial.print(client.state());*/
      Serial.println(" try on next loop");     
    }
}



float getTemperature() {
  Serial.println ("Requesting DS18B20 temperature..."); 
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void sendtemp(){
   unsigned long now = millis();
  // function to send the temperature every five minutes rather than leavingb in the loop
   if (now - lastSampleTime >= fiveMinutes)
  {
     float temperature = getTemperature();
  // convert temperature to a string with two digits before the comma and 2 digits for precision
  dtostrf(temperature, 2, 2, temperatureString);
  // send temperature to the serial console
  Serial.println ("Sending temperature: ");
  Serial.println (temperatureString);
  // send temperature to the MQTT topic every 5 minutes
     client.publish(roomtemp, temperatureString);
  //lastSampleTime = now + fiveMinutes;
  lastSampleTime += fiveMinutes;
   // add code to take scroll temperatre 4 times then reset face to static  (temperature is 5 characters * 8  for each scroll
  ledMatrix.setText(temperatureString);
  for (int i=0; i < 160; i++){
  ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(100);
   }
  }
}

 
void setup() {
   // setup serial port
  Serial.begin(115200);
  delay(10);
  pinMode(buttonPin, INPUT);
  
  setup_wifi(); // setup wifi
  DS18B20.begin(); //Setup Ds18B20
  ledMatrix.init(); // setup Max27219
  ledMatrix.setIntensity(4); // Set Brightness of LED
  ledMatrix.setCharWidth(8); // Set Character Width to get spacing correct
 // client.set_callback(callback);

}

void loop() {
  // put your main code here, to run repeatedly:
   client.set_callback(callback);
   if (!client.connected()) {
    reconnect();
  }
  

  client.loop();
 
  
 // client.publish(roomtemp, temperatureString);
  client.subscribe(messages);
  client.subscribe(dinner);
  client.subscribe(dick);
 // This is where display temperature every five minutes
 sendtemp();
 
  /* Test the animation loop
  if (buttonState == 0) {
      animate_face(1);
      buttonState = 1;
  }*/

  

}
