# Wireless Industrial Sensor Network

A two-node wireless sensor network built on ESP32 microcontrollers, simulating the architecture of industrial facility monitoring systems used in oil and gas, petrochemical, and process engineering environments.

Field sensor nodes detect hazardous conditions and transmit data wirelessly to a central base station for real-time monitoring and alerting. This mirrors the fundamental design of wireless sensor networks deployed across industrial sites for personnel safety, asset protection, and environmental compliance.

## System Architecture

**Node 1 — Field Sensor Node (Arduino Nano ESP32)**

Reads three sensors continuously and broadcasts a compact UDP packet over WiFi every second to any listening device on the network.

Sensors:
- MQ-135 gas sensor — detects elevated levels of harmful gases including CO2, ammonia, and benzene
- HC-SR04 ultrasonic sensor — detects objects or personnel within a configurable proximity threshold
- PIR motion sensor — detects movement in the monitored area

**Node 2 — Base Station (ESP32-S3 DevKitC)**

Listens for incoming UDP packets, parses the sensor data, and displays live readings on an OLED screen. Triggers a buzzer alert when any parameter breaches a safe threshold. Powered independently via 9V battery, requiring no laptop or USB connection.

## Hardware

Node 1:
- Arduino Nano ESP32
- MQ-135 gas sensor module
- HC-SR04 ultrasonic distance sensor
- PIR motion sensor module
- 2x 10k ohm resistors for voltage divider on HC-SR04 echo pin

Node 2:
- ESP32-S3 DevKitC-1
- SSD1306 128x64 OLED display
- Active buzzer
- 9V battery

## Wiring

**Node 1**

MQ-135: VCC to VBUS, GND to GND, AO to A0

HC-SR04: VCC to VBUS, GND to GND, TRIG to D4, ECHO through a 10k ohm voltage divider to D5

PIR: VCC to VBUS, GND to GND, OUT to D3

**Node 2**

SSD1306 OLED: VCC to 3.3V, GND to GND, SDA to GPIO8, SCL to GPIO9

Buzzer: positive to GPIO5, negative to GND

Power: 9V battery positive to 5V pin, negative to GND

## Alert Thresholds

Gas level above 800 raw ADC triggers a gas alert. The baseline in clean air reads around 700, so anything above 800 indicates elevated gas concentration.

Distance below 20 cm triggers a proximity alert, indicating an object or person has entered the monitored zone.

Motion detection from the PIR triggers an alert immediately.

All thresholds are defined as constants at the top of each sketch and can be adjusted for different environments.

## UDP Packet Format

Node 1 broadcasts a plain text packet every second in the following format: M:x,D:xx.x,G:xxxx 
Where M is motion (0 or 1), D is distance in cm, and G is the raw gas ADC value.

## Libraries Required

- Adafruit SSD1306
- Adafruit GFX Library

Install via Arduino IDE Library Manager.

## Industrial Relevance

The architecture of this project directly reflects real-world industrial sensor network design.

Wireless sensor nodes transmitting to a central receiver mirrors downhole telemetry systems such as MWD tools used in oil and gas drilling operations. Gas detection is a core safety requirement on hydrocarbon processing sites, where continuous monitoring for H2S, methane, and VOCs is mandatory. Proximity and motion detection mirrors perimeter security and access control systems used on oil and gas facilities. The UDP broadcast protocol reflects the low-latency one-to-many communication patterns used in industrial SCADA and telemetry systems.

## Author

Fares Mahgoub
github.com/FaresMahgoub
