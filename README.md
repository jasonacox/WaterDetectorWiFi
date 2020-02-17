# WaterDetectorWiFi
ESP8266 NodeMCU Based IoT Water Detector with Temperature Probe

## Description
This project describes how to build a device to detect water and post the status to a URL or MQTT endpoints. 
We will use a ESP8266 NodeMCU Arduino based controller with a simple water detection circuit using a 2N2222 NPN transitor, 
1K ohm reistor and a 100uF electrolytic capacitor (with optional LED for indicator).  Multiple water probes can be added to measure water level.

Optional temperature probe: Adding a DS18B20 One-Wire bus temperature sensor to read and send current temperature.

## Circuit
This project is based on the amazing low-cost WiFi enable ESP8266 NodeMCU controller.  We will use the GPIO digital ports, specifically:
* GPIO 13 (pin D7) - Water Sensor #1
* GPIO 15 (pin D8) - Water Sensor #2
* GPIO 14 (pin D5) - External LED Indicator
* GPIO  4 (pin D2) - OneWire Bus for DS18B20 Temperature Probe

![Circuit Board](schematic.png)

### Water Sensors
Requires: water resistant 2 prong electrode, 1K ohm reistor and a 100uF electrolytic capacitor 

### Temperature Sensor
Requires: DS18B20 temperature sensor (recommend waterproof probe) and 5.1K ohm resistor

## Code
TBD

## Reference
ESP8266 NodeMCU - Amazon: https://www.amazon.com/gp/product/B07XVKJ36C/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
Water Induction Electrode - Amazon: https://www.amazon.com/gp/product/B014SACDSO/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
DS18b20 Waterproof Temperature Sensors - Amazon: https://www.amazon.com/gp/product/B00CHEZ250/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

