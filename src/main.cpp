#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "camera.h"
#include "esp_timer.h"
#include "motors.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

// Replace with your network credentials
String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
String ssid = "MBotMini-" + chipId;
const char* password = "makerbotisfun";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
unsigned int controlPort = 9999;

WiFiUDP Udp;  // Creation of wifi Udp instance
char packetBuffer[255];
unsigned long lastSignalTime;  // Last signal from computer

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_CIF;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid.c_str(), password);
  delay(500);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  startCameraServer();
  Udp.begin(controlPort);

  // Motors
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