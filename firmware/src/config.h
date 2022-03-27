
#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

#define CAMERA_FRAME_SIZE FRAMESIZE_CIF
#define CAMERA_QUALITY 10

#define MOTOR1_A 15
#define MOTOR1_B 14
#define MOTOR2_A 13
#define MOTOR2_B 12

String CHIP_ID = String((uint32_t)ESP.getEfuseMac(), HEX);
String WIFI_SSID = "MBotMini-" + CHIP_ID;
const char* WIFI_PASSWORD = "makerbotisfun";

IPAddress AP_IP(192, 168, 4, 1);
IPAddress NET_MASK(255, 255, 255, 0);
unsigned int CONTROL_PORT = 9999;

#endif