/*
 * Node 1: Wireless Field Sensor Node
 *
 * Simulates an industrial wireless sensor node as used in oil and gas
 * facility monitoring. Detects hazardous gas levels, unauthorized
 * proximity, and motion. Broadcasts data over WiFi UDP to a base
 * station for real-time monitoring and alerting.
 *
 * Hardware: Arduino Nano ESP32
 * Sensors: MQ-135 (gas), HC-SR04 (proximity), PIR (motion)
 */

#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Mobily2025";
const char* password = "Omnia2025";

WiFiUDP udp;
const int UDP_PORT = 4210;
const char* BROADCAST_IP = "255.255.255.255";

// Pin definitions
#define PIR_PIN    3      // PIR motion sensor output
#define TRIG_PIN   4      // HC-SR04 trigger
#define ECHO_PIN   5      // HC-SR04 echo (voltage divided to 3.3V)
#define MQ135_PIN  A0     // MQ-135 analog output

// Alert thresholds
// Gas threshold is a raw ADC value (0 to 4095 on 12-bit ESP32 ADC)
// Baseline clean air reads around 700. Above 800 indicates elevated gas.
#define GAS_THRESHOLD   800
// Proximity threshold in cm. Object closer than this triggers an alert.
#define DIST_THRESHOLD  20.0

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(UDP_PORT);
  Serial.println("Node 1 active. Broadcasting sensor data.");
}

// Measures distance in cm using HC-SR04 ultrasonic sensor.
// Returns -1 if no echo received within timeout.
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2.0;
}

void loop() {
  int motion = digitalRead(PIR_PIN);
  float distance = readDistance();
  int gasLevel = analogRead(MQ135_PIN);

  // Build compact packet string for UDP transmission
  // Format: M:x,D:xx.x,G:xxxx
  char packet[128];
  snprintf(packet, sizeof(packet),
    "M:%d,D:%.1f,G:%d",
    motion, distance, gasLevel);

  // Broadcast to all devices on the network
  udp.beginPacket(BROADCAST_IP, UDP_PORT);
  udp.print(packet);
  udp.endPacket();

  Serial.print("Sent: ");
  Serial.println(packet);

  delay(1000);
}