#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <CytronWiFiServer.h>
#include <SoftwareSerial.h>
#define sensorPin A0
#define lightsensor A1

const char *ssid = "";  // **replace the SSID
const char *pass = "";  // **replace the password

ESP8266Client client;

int celcius = 0;
int light = 0;
unsigned long start, finished, elapsed;
String apikey = "";    // **replace with your api key from the FAVORIOT platform account setting
char serverAdd[] = "api.favoriot.com";

void setup() {
   Serial.begin(9600);  
   pinMode(sensorPin,INPUT);
   pinMode(lightsensor,INPUT);

   if(!wifi.begin(2, 3))
  {
    Serial.println(F("Error talking to shield"));
    while(2000);
  }
  
  Serial.println(wifi.firmwareVersion());
  Serial.print(F("Mode: "));Serial.println(wifi.getMode());// 1- station mode, 2- softap mode, 3- both
  Serial.println(F("Setup wifi config"));
  Serial.println(F("Start wifi connection to"));
  Serial.print(F("Wifi SSID: "));Serial.println(ssid);
 
  if(!wifi.connectAP(ssid, pass))
  {
    Serial.println(F("Error connecting to WiFi"));
    while(2000);
  } 
  
  Serial.print(F("Connected to "));Serial.println(wifi.SSID());
  Serial.println(F("IP address: "));
  Serial.println(wifi.localIP());
  wifi.updateStatus();
  Serial.println(wifi.status()); //2- wifi connected with ip, 3- got connection with servers or clients, 4- disconnect with clients or servers, 5- no wifi

}

void loop() {
  celcius = analogRead(sensorPin) * 0.488;  //read the temperature and convert to celcius
     light  = analogRead(lightsensor); 
     start=millis();  //start counting time
     dataStream(celcius,light);  //send data to FAVORIOT platform
     finished=millis();  //stop counting time
     elapsed=(finished-start) / 1000;  //calculate time taken to send data
     Serial.print("        PROCESSING TIME : " + String(elapsed) + " seconds\n");  //display time taken
     Serial.print("**************************************************");
     delay(10000);

}

void dataStream(int celcius, int light)
{ 
  String json = "{";
    json += "\"device_developer_id\":"; 
    json += "\"deviceDefault@favoriotplatform\"";
    json +=",";
    json +="\"data\":";
    json += "{";
    json += "\"Temperature\":";
    json += "\""+String(celcius)+"\"";
    json += ",";
    json += "\"Light\":";
    json += '"'+String(light)+'"';
    json += "}}";
      
  if (client.connect(serverAdd, 80)) {
    Serial.println("        STATUS : Sending data...");  //Display sending status
    client.println("POST /v1/streams HTTP/1.1");
    client.println("Host: api.favoriot.com");
    client.println("Connection:close");
    client.print(String("apikey: "));
    client.println(apikey);
    client.println("Content-Type: application/json");
    client.println("cache-control: no-cache");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println();
    client.println(json);
    Serial.println("        STATUS : Data sent!");  //display sent status
    }
}
