//Arduino side code
//DHT11 Lib
#include <DHT.h>

//Watchdog Timer
#include <avr/wdt.h>

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);

//Initialisation of DHT11 Sensor
#define DHTPIN 4
DHT dht(DHTPIN, DHT11);
float temp;
float hum;

//Timers millis
unsigned long previousMillis = 0;
unsigned long currentMillis;
const unsigned long period = 30000;

void setup() {
  Serial.begin(9600);
  dht.begin();
  nodemcu.begin(9600);
  delay(2000);

  //Enable Watchdog timer
  wdt_enable(WDTO_4S);

  Serial.println("Program started");
}

void loop() {
  //Get current time
  currentMillis = millis();

  if ((currentMillis - previousMillis >= period)) {
    StaticJsonDocument<1000> doc;

    //Obtain Temp and Hum data
    dht11_func();

    //Assign collected data to JSON Object
    doc["humidity"] = hum;
    doc["temperature"] = temp;

    //Send data to NodeMCU
    serializeJson(doc, nodemcu);

    previousMillis = previousMillis + period;
  }

  //Reset for WDT
  wdt_reset();
}

void dht11_func() {

  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp);

  if (isnan(hum) || isnan(temp)|| hum == 0.0 || temp == 0.0) {
    while (1) {}
  }
}
