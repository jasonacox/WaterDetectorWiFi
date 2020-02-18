/*
    WaterDetectorWiFi
    ESP8266 NodeMCU Based IoT Water Detector with Temperature Probe

    Poll water sensors and one-wire tempature sensor and send results to web or mqtt server
    
    Author: Jason A. Cox - @jasonacox - https://github.com/jasonacox/WaterDetectorWiFi

    Date: 17 Feb 2020
    
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <OneWire.h>
#include <DallasTemperature.h> /* For one-wire temp probe */
// TO DO - Add MQTT

#ifndef WIFISSID
#define WIFISSID "Your Network SSID"
#define WIFIPASS "Your Password"
#endif

#define SENSORID 100   // Serial Number - Unqiue for IoT Device

/* Pins */
#define LED 14         // LED on GPIO14 = D5
#define WATER_1 13     // Water Sensor NPN Transitor on GPIO13 = D7
#define WATER_2 12     // Water Sensor NPN Transitor on GPIO12 = D6
#define ONEWIRE 4      // OneWire Buss on GPIO4 = D2

/* States */
#define DRY 0
#define WET 1
#define FLOOD 2
#define NIL -1

/* Debug Mode? Verbose Output */
#define DEBUG 1

/* Wifi Settings */
const char* ssid     = WIFISSID;
const char* password = WIFIPASS;

/* Server location to send alerts and status */
const char* host = "10.10.10.10";  // HTTP Server
const uint16_t port = 80;
#define URIPREFIX "/sensor.php"    // URI to send updates
const char* mqtt = "10.10.10.10";  // MQTT Server 
const uint16_t mqttport = 1883;

/* globals */
int state;
int count;
ESP8266WiFiMulti WiFiMulti;
OneWire oneWire(ONEWIRE);             // DS18B20 One-Wire Temp Probe
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
DeviceAddress Thermometer;            // variable to hold device addresses
int deviceCount = 0;
// uint8_t sensor1[8] = { 0x28, 0xFF, 0xAB, 0x06, 0x81, 0x14, 0x02, 0xA0  };

/*
 * SETUP - Runs on Startup ONLY
 */
void setup() {
  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(WATER_1, INPUT_PULLUP);
  pinMode(WATER_2, INPUT_PULLUP);
    
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED, HIGH);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    digitalWrite(LED, LOW);  
    delay(10);                       
    digitalWrite(LED, HIGH); 
    delay(10);                     
    digitalWrite(LED, LOW);  
    delay(10);                       
    digitalWrite(LED, HIGH); 
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  digitalWrite(LED, LOW);
  delay(500);

  state = NIL;
  count = 0;

  /* fire up one-wire temp probe and get list of devices available */
  // Start up the library for DS18B20 One-Wire Temp Probe
  sensors.begin();

  // locate devices on the bus
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
  
  Serial.println("Printing addresses...");
  for (int i = 0;  i < deviceCount;  i++)
  {
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    sensors.getAddress(Thermometer, i);
    printAddress(Thermometer);
  }
}

/*
 * MAIN LOOP 
 */
void loop() {

  int sensor1 = 0;
  int sensor2 = 0;

  /* Force heartbeat every 300 cycles ~= 5m */
  count++;
  if(count > 300) {
     count = 0;
     state = NIL;
  }
  
  // Heartbeat - flash onboard LED
  digitalWrite(LED_BUILTIN, LOW);  
  delay(10);                       
  digitalWrite(LED_BUILTIN, HIGH); 

  // Capture current temp
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  if(DEBUG) {
    Serial.print("     : Temp = ");
    Serial.print(temperatureC);
    Serial.print("ºC / ");
    float temperatureF = sensors.getTempFByIndex(0);
    Serial.print(temperatureF);
    Serial.println("ºF");
  }
  
  // Poll water sensors
  sensor1 = digitalRead(WATER_1); // waterlevel alert 1
  sensor2 = digitalRead(WATER_2); // waterlevel alert 2
  if(DEBUG) {
    Serial.print("STATE: ");
    Serial.print(state, DEC);
    Serial.print(" - Water Sensors: Location 1 = ");  
    Serial.print(sensor1, DEC);
    Serial.print(" - Location 2 = ");
    Serial.println(sensor2, DEC);
  }
  
  // Detected water at sensor1 = low water warning
  if(sensor1 == 0 and state < WET) {
    digitalWrite(LED, HIGH);
    Serial.println("**  Water Detected - Low Water Warning  **");
    state = WET;

    // Push update to server
    sendStatus(1, SENSORID, temperatureC);
  }

  // Detected water at sensor2 = highwater warning
  if(sensor2 == 0 and state != FLOOD) {
    digitalWrite(LED, HIGH);
    Serial.println("**  Flood Detected - High Water Warning  **");
    state = FLOOD;

    // Push update to server
    sendStatus(2, SENSORID, temperatureC);
  }

  // Sensor2 clears but sensor1 still alerting = downgrade to low water warning
  if(sensor1 == 0 and sensor2 == 1 and state == FLOOD) {
        digitalWrite(LED, HIGH);
    Serial.println("**  Downgrade to Low Water Warning  **");
    state = WET;

    // Push update to server
    sendStatus(1, SENSORID, temperatureC);
  }

  // Sensor1 and Sensor2 clear 
  if(sensor1 == 1 and sensor2 == 1 and state != DRY) {
    digitalWrite(LED, LOW);
    Serial.println("**  Sensor Dry - All Clear  **");
    state = DRY;
   
    // Push update to server
    sendStatus(0, SENSORID, temperatureC);
  }
  
  // Wait about a second
  delay(990);
}

/*
 * Prints OneWire Bus Device Address
 */
void printAddress(DeviceAddress deviceAddress)
{ 
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}

/*
 * Send Status to Web Server - water level and temperature
 *    waterlevel: 1=wet, 0=dry
 *    temp: floating point degrees celcius
 */
void sendStatus(int waterlevel, int id, float temp)
{
    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    // Send via HTTP GET
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      Serial.println("wait 5 sec...");
      delay(5000);
      return;
    }
    client.print("GET ");
    client.print(URIPREFIX);
    client.print("?waterlevel=");
    client.print(waterlevel);
    client.print("&id=");
    client.print(id);
    client.print("&temp=");
    client.print(temp);
    client.println(" HTTP/1.0");
    client.println();

    //read back one line from server
    Serial.println("receiving from remote server");
    String line = client.readStringUntil('\r');
    Serial.println(line);
  
    Serial.println("closing connection");
    client.stop();

    // Send via MQTT
    // TBD
}
