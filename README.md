# WaterDetectorWiFi
ESP8266 NodeMCU Based IoT Water Detector

## Description
This project describes how to build a device to detect water and post the status to a URL or MQTT endpoint. 
We will use a ESP8266 NodeMCU Arduino based controller with a simple water detection circuit using a 2N2222 NPN transitor, 
1K ohm reistor and a 100uF electrolytic capacitor (with optional LED for indicator).

## Circuit
This project is based on the amazing low-cost WiFi enable ESP8266 NodeMCU controller.  We will use two of the GPIO 
digital ports, specifically:
* GPIO 13 (pin D7) - Wet Sensor Line
* GPIO 14 (pin D5) - External LED Indicator

## Code

## Reference
ESP8266 NodeMCU - Amazon: https://www.amazon.com/gp/product/B07XVKJ36C/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
