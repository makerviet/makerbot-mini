#ifndef MOTORS_H_
#define MOTORS_H_

#include <stdio.h>

#include "config.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define MOTOR_FREQ 5000  // PWM MOTOR_FREQ for DC motor should be within 5-20kHz

#define PWM_CHANNEL1 LEDC_CHANNEL_2
#define PWM_CHANNEL2 LEDC_CHANNEL_3

#define PWM_CHANNEL3 LEDC_CHANNEL_4
#define PWM_CHANNEL4 LEDC_CHANNEL_5

#define PWM_RES \
  LEDC_TIMER_8_BIT  // Resolution of pwm signal here is set to 8 bit, resolution
                    // can be set up to 15 bit with esp32

#define MIN_PWM 0
#define MAX_PWM 255

void setPWMMotors(float c1, float c2, float c3, float c4) {
  ledcWrite(PWM_CHANNEL1, c1);
  ledcWrite(PWM_CHANNEL2, c2);
  ledcWrite(PWM_CHANNEL3, c3);
  ledcWrite(PWM_CHANNEL4, c4);
}

void initMotors() {
  ledcSetup(PWM_CHANNEL1, MOTOR_FREQ, PWM_RES);
  ledcSetup(PWM_CHANNEL2, MOTOR_FREQ, PWM_RES);
  ledcSetup(PWM_CHANNEL3, MOTOR_FREQ, PWM_RES);
  ledcSetup(PWM_CHANNEL4, MOTOR_FREQ, PWM_RES);

  ledcAttachPin(MOTOR1_A, PWM_CHANNEL1);
  ledcAttachPin(MOTOR1_B, PWM_CHANNEL2);
  ledcAttachPin(MOTOR2_A, PWM_CHANNEL3);
  ledcAttachPin(MOTOR2_B, PWM_CHANNEL4);

  setPWMMotors(0, 0, 0, 0);
}

/**
 * Set speed and direction for 2 motors
 *
 * @param left_motor_speed: speed with direction for left motor. Range from -1
 * to 1. 1: max speed forward, -1: max speed reverse
 * @param right_motor_speed: speed with direction for right motor. Range from -1
 * to 1. 1: max speed forward, -1: max speed reverse
 */
void setSpeed(float left_motor_speed, float right_motor_speed) {
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

  if (left_motor_speed > 0) {
    c1 = max(MIN_PWM, min(int(abs(left_motor_speed) * MAX_PWM), MAX_PWM));
  } else {
    c2 = max(MIN_PWM, min(int(abs(left_motor_speed) * MAX_PWM), MAX_PWM));
  }

  if (right_motor_speed > 0) {
    c3 = max(MIN_PWM, min(int(abs(right_motor_speed) * MAX_PWM), MAX_PWM));
  } else {
    c4 = max(MIN_PWM, min(int(abs(right_motor_speed) * MAX_PWM), MAX_PWM));
  }

  setPWMMotors(c1, c2, c3, c4);
}

#endif