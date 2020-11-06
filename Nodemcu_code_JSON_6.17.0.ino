//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

//Timer to run Arduino code every 5 seconds
unsigned long previousMillis = 0;
unsigned long currentMillis;
const unsigned long period = 10000;  

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  while (!Serial) continue;
}

void loop() {
  //Get current time
  currentMillis = millis();

  if ((currentMillis - previousMillis >= period)) {

  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, nodemcu);

  // Test parsing
  while (error) {
    Serial.println("Invalid JSON Object");
    delay(500);
    DeserializationError error = deserializeJson(doc, nodemcu);
  }

  Serial.println("JSON Object Recieved");
  Serial.print("Recieved Humidity:  ");
  float hum = doc["humidity"];
  Serial.println(hum);
  Serial.print("Recieved Temperature:  ");
  float temp = doc["temperature"];
  Serial.println(temp);
  Serial.println("-----------------------------------------");

previousMillis = previousMillis + period;
  }
}
