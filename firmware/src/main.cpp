#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "camera.h"
#include "config.h"
#include "motors.h"
#include "soc/rtc_cntl_reg.h"

WiFiUDP Udp;
char packetBuffer[255];

// Save last time receiving signal from control client to stop on connection lost
unsigned long lastSignalTime;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector

  // Setup serial communication with computer
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  // Initialize camera
  initCamera();

  // Setup WIFI
  WiFi.softAPConfig(AP_IP, AP_IP, NET_MASK);
  WiFi.softAP(WIFI_SSID.c_str(), WIFI_PASSWORD);
  delay(1000);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start camera server for streaming video
  // UDP for receiving control command
  startCameraServer();
  Udp.begin(CONTROL_PORT);

  // Setup motors
  // See config.h for pin setup
  initMotors();
}

// Value range: [-100, 100]
// 0 -> 100: Forward
// 0 -> -100: Backward
void controlWheels(int16_t leftWheel, int16_t rightWheel) {
  Serial.println(String("Control ") + String(leftWheel) + " " +
                 String(rightWheel));
  double dLeftWheel = (double)leftWheel;
  double dRightWheel = (double)rightWheel;
  setSpeed(dLeftWheel / 100.0, dRightWheel / 100.0);
}

void loop() {
  // Receive control commands
  int packetSize = Udp.parsePacket();  // Size of packet to receive
  if (packetSize) {                    // If we received a package

    // Stop if the control signal is lost
    if (millis() - lastSignalTime > 1000) {
      controlWheels(0, 0);
    }

    // Parse UDP packet
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    char* token = strtok(packetBuffer, " ");
    if (strcasecmp(token, "CONTROL_WHEEL") == 0) {  // Packet to set wheel speed
      token = strtok(NULL, " ");
      int16_t leftWheel = atoi(token);
      token = strtok(NULL, " ");
      int16_t rightWheel = atoi(token);
      controlWheels(leftWheel, rightWheel);
    }
    lastSignalTime = millis();
  }
}