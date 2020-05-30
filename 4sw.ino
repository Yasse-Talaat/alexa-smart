#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

WebSocketsClient webSocket;
WiFiClient client;
#include <EEPROM.h>

boolean state[] = {1};
int val2 = 0, val = 0;

#define MyApiKey "api code here" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

#define lamp1 "5d4ea1e1ea25201d63c7da3f"
#define lamp2 "5ed1b8fcd8c9f16bc25c8f2d" 
#define lamp3 "5ed1b90ed8c9f16bc25c8f31"
#define lamp4 "5ed1b91ed8c9f16bc25c8f34"

const int ligh1=13, ligh2=12, ligh3=2, ligh4=0;
int state1; int state2; int state3; int state4;
int swState1; int swState2; int swState3; int swState4;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

void setup() {
  Serial.begin(115200);
  EEPROM.begin(1024);
  pinMode (ligh1,OUTPUT);
  pinMode (ligh2,OUTPUT);
  pinMode (ligh3,OUTPUT);
  pinMode (ligh4,OUTPUT);
  digitalWrite (ligh1,HIGH);
  digitalWrite (ligh2,HIGH);
  digitalWrite (ligh3,HIGH);
  digitalWrite (ligh4,HIGH);
  
  pinMode (14,INPUT);
  pinMode (4,INPUT);
  pinMode (5,INPUT);
  pinMode (16,INPUT);
  pinMode (17,INPUT);

  WiFiManager wifiManager;
  wifiManager.autoConnect("Be Smart");
  webSocket.begin("iot.sinric.com", 80, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  webSocket.setReconnectInterval(5000); 


  
  if (EEPROM.read(100) == 0) { digitalWrite (ligh1,LOW); setPowerStateOnServer(lamp1, "ON");}
  delay(30); if (EEPROM.read(101) == 0) { digitalWrite (ligh2,LOW); setPowerStateOnServer(lamp2, "ON");}
  delay(30); if (EEPROM.read(102) == 0) { digitalWrite (ligh3,LOW); setPowerStateOnServer(lamp3, "ON");}
  delay(30); if (EEPROM.read(103) == 0) { digitalWrite (ligh4,LOW); setPowerStateOnServer(lamp4, "ON");}
  Serial.println("Yasso");
}


void loop() {

int sw1=digitalRead(14); int sw2=digitalRead(4); int sw3=digitalRead(5); int sw4=digitalRead(16);

if (sw1==0 && swState1==0) {digitalWrite(ligh1, !digitalRead(ligh1)); swState1 = 1;
if (digitalRead(ligh1)==HIGH) { setPowerStateOnServer(lamp1, "OFF"); EEPROM.write (100, 0); }}
else if (sw1==1 && swState1==1) { digitalWrite(ligh1,!digitalRead(ligh1)); swState1 = 0 ;
if (digitalRead(ligh1)==HIGH) { setPowerStateOnServer(lamp1, "OFF"); EEPROM.write (100, 0); }}

if (sw2==0 && swState2==0) {digitalWrite(ligh2,!digitalRead(ligh2)); swState2 = 1;
if (digitalRead(ligh2)==HIGH) { setPowerStateOnServer(lamp2, "OFF"); EEPROM.write(101, 0);}}
else if (sw2==1 && swState2==1) { digitalWrite(ligh2,!digitalRead(ligh2)); swState2 = 0 ;
if (digitalRead(ligh2)==HIGH) { setPowerStateOnServer(lamp2, "OFF"); EEPROM.write(101, 0); }}

if (sw3==0 && swState3==0) {digitalWrite(ligh3,!digitalRead(ligh3)); swState3 = 1;
if (digitalRead(ligh3)==HIGH) { setPowerStateOnServer(lamp3, "OFF"); EEPROM.write(102, 0);}}
else if (sw3==1 && swState3==1) { digitalWrite(ligh3,!digitalRead(ligh3)); swState3 = 0 ;
if (digitalRead(ligh3)==HIGH) { setPowerStateOnServer(lamp3, "OFF"); EEPROM.write(102, 0); }}

if (sw4==0 && swState4==0) {digitalWrite(ligh4,!digitalRead(ligh4)); swState4 = 1;
if (digitalRead(ligh4)==HIGH) { setPowerStateOnServer(lamp4, "OFF"); EEPROM.write(103, 0);}}
else if (sw4==1 && swState4==1) { digitalWrite(ligh4,!digitalRead(ligh4)); swState4 = 0 ;
if (digitalRead(ligh4)==HIGH) { setPowerStateOnServer(lamp4, "OFF"); EEPROM.write(103, 0); }}



webSocket.loop();
if(isConnected) { uint64_t now = millis(); if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
heartbeatTimestamp = now; webSocket.sendTXT("H"); }}   
delay(10);
}


void turnOn(String deviceId) {
  if (deviceId == lamp1) { Serial.print("Turn on device id: "); Serial.println(deviceId); digitalWrite (ligh1,HIGH); EEPROM.write(100, 1); EEPROM.commit(); } 
  else if (deviceId == lamp2) { Serial.print("Turn on device id: "); Serial.println(deviceId); digitalWrite (ligh2,HIGH); EEPROM.write(101, 1); EEPROM.commit(); }
  else if (deviceId == lamp3) { Serial.print("Turn on device id: "); Serial.println(deviceId); digitalWrite (ligh3,HIGH); EEPROM.write(102, 1); EEPROM.commit(); }
  else if (deviceId == lamp4) { Serial.print("Turn on device id: "); Serial.println(deviceId); digitalWrite (ligh4,HIGH); EEPROM.write(103, 1); EEPROM.commit(); }
  else  { Serial.print("Turn on for unknown device id: "); Serial.println(deviceId);}
}

void turnOff(String deviceId) {
  if (deviceId == lamp1) { Serial.print("Turn off device id: "); Serial.println(deviceId); digitalWrite (ligh1,LOW); EEPROM.write(100, 0); EEPROM.commit(); } 
  else if (deviceId == lamp2) { Serial.print("Turn off device id: "); Serial.println(deviceId); digitalWrite (ligh2,LOW); EEPROM.write(101, 0); EEPROM.commit(); }
  else if (deviceId == lamp3) { Serial.print("Turn off device id: "); Serial.println(deviceId); digitalWrite (ligh3,LOW); EEPROM.write(102, 0); EEPROM.commit(); }
  else if (deviceId == lamp4) { Serial.print("Turn off device id: "); Serial.println(deviceId); digitalWrite (ligh4,LOW); EEPROM.write(103, 0); EEPROM.commit(); }   
  else  { Serial.print("Turn off for unknown device id: "); Serial.println(deviceId);}

}



void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}



void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  webSocket.sendTXT(databuf);
}

void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}
