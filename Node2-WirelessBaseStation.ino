/*
 * Node 2: Wireless Base Station
 *
 * Receives sensor data broadcast over WiFi UDP from field sensor nodes.
 * Parses incoming packets, displays live readings on an OLED screen,
 * and triggers a buzzer alert when any parameter exceeds a safe threshold.
 *
 * Mirrors the role of a control room monitoring station in industrial
 * oil and gas facilities, where remote sensor nodes transmit data to
 * a central receiver for real-time situational awareness.
 *
 * Hardware: ESP32-S3 DevKitC
 * Output: SSD1306 OLED display, active buzzer
 */

#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "Mobily2025";
const char* password = "Omnia2025";

WiFiUDP udp;
const int UDP_PORT = 4210;

// OLED display configuration
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C pins on ESP32-S3 DevKitC
#define SDA_PIN    8
#define SCL_PIN    9

// Buzzer pin
#define BUZZER_PIN 5

// Alert thresholds
// Gas threshold matches Node 1 baseline. Above 800 indicates elevated gas.
#define GAS_THRESHOLD   800
// Proximity threshold in cm. Intruder within 20cm triggers alert.
#define DIST_THRESHOLD  20.0

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialise I2C with custom pins for ESP32-S3
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialise OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Base Station Ready");
  display.display();

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
  Serial.println("Node 2 active. Listening for packets.");
}

// Sounds three short buzzer beeps to indicate an alert condition.
// Used when gas, proximity, or motion threshold is breached.
void triggerAlert() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buffer[128];
    int len = udp.read(buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    Serial.print("Received: ");
    Serial.println(buffer);

    // Parse incoming packet
    // Expected format: M:x,D:xx.x,G:xxxx
    int motion = 0;
    float distance = 0;
    int gasLevel = 0;
    sscanf(buffer, "M:%d,D:%f,G:%d", &motion, &distance, &gasLevel);

    // Evaluate alert conditions
    bool alert = false;
    if (gasLevel > GAS_THRESHOLD) alert = true;
    if (distance > 0 && distance < DIST_THRESHOLD) alert = true;
    if (motion == 1) alert = true;

    // Update OLED with latest readings
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Gas Level: ");
    display.println(gasLevel);

    display.setCursor(0, 16);
    display.print("Distance: ");
    display.print(distance, 1);
    display.println(" cm");

    display.setCursor(0, 32);
    display.print("Motion: ");
    display.println(motion ? "DETECTED" : "None");

    display.setCursor(0, 48);
    if (alert) {
      display.println("!! ALERT !!");
    } else {
      display.println("Status: Normal");
    }

    display.display();

    if (alert) triggerAlert();
  }
}