//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Wifimodule and thingspeak lib
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

//Timers millis
unsigned long previousMillis = 0;
unsigned long currentMillis;
const unsigned long period = 120000;

//Variable for data
float temp;
float hum;
int ldr;

//Wifi related variables
String apiKey = "R87NASPSTHXH6XFY";     //Write API key
//const char * ReadAPIKey = "P56PFN1K4WXFUD25";

unsigned long channelID = 1220498;
const char *ssid =  "Rosh wifi";
const char *pass =  "chamslan12";
const char* server = "api.thingspeak.com";

//Boolean Variables
boolean state = false;
boolean state2 = false;

//Field 1 = Temperature
//Field 2 = Humidity
//Field 3 = LDR

WiFiClient client;

//-------------------------------------------------- SETUP ------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  ThingSpeak.begin(client);
  while (!Serial) continue;
  delay(2000);

  //Enable Watchdog timer
  //ESP.wdtEnable(4000);

  Serial.println("Program Started");

  //Establish WiFi Connection
  wifi_connect();
}

//-------------------------------------------------- LOOP ------------------------------------------------------------

void loop() {
  //Get current time
  currentMillis = millis();

  if ((currentMillis - previousMillis >= period)) {

    //Check if connected to WiFi, else connect
    if (WiFi.status() != WL_CONNECTED) {
      wifi_connect();
    }
    else {
      Serial.println("Currently Connected to WiFi");
    }

    //Connect to Thingspeak and push data
    while (state == false) {

      while (state2 == false) {
        parseJsonObject();
      }
      
      cloud_connect();
    }

    state = false;
    state2 = false;
    Serial.println("Cloud Connect Succesful");
    Serial.println("-----------------------------------------");
    previousMillis = previousMillis + period;
  }
}

//----------------------------------------- FUNCTION - WiFi Connect -----------------------------------------

void wifi_connect() {
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  //ThingSpeak.begin(client);
  Serial.println("WiFi Connected");
}

//------------------------------------------ FUNCTION Cloud Connect -----------------------------------------

void cloud_connect() {
  //"184.106.153.149" or api.thingspeak.com
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "&field2=";
    postStr += String(hum);
    //postStr += "&field3=";
    //postStr += String(ldr);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" || Humidity: ");
    Serial.println(hum);
    //Serial.print(" || LDR: ");
    //Serial.println(ldr);
    Serial.println("Sent to Thingspeak");
    
    //Change state to true to break while loop
  state = true;
  }
  client.stop();
}

void parseJsonObject() {
  //Parse JSON object from serial port
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, nodemcu);

  // Test if parsing succeeds
  if (error) {
    Serial.println("deserializeJson error");
    return;
  }

  //Parse Json values and store in variable
  hum = doc["humidity"];
  temp = doc["temperature"];
  //ldr = doc["ldr"];

  if (isnan(hum) || isnan(temp) || hum == 0.0 || temp == 0.0) {
    Serial.println("Invalid Sensor Readings");
    return;
  }

  state2 = true;
  Serial.println("JSON Object Recieved");
}
