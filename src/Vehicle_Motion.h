#ifndef VEHICLE_MOTION_H
#define VEHICLE_MOTION_H

//includes adafruit motorshield
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Arduino_LSM6DS3.h>
#include <Servo.h>
#include "Sensors.h"

//frequency of flashing
#define FLASH_freq 2
//digital LED pins
#define GLED_pin 2
#define YLED_pin 0
#define RLED_pin 1
//servo pins
#define Servo_pin 9

extern Adafruit_MotorShield AFMS;
extern int mode, drop_count, triple_count;
extern bool PICKED;
extern bool DROPPED;
extern unsigned int touch;

void set_mode(void);
void vehicle_stop(void);
void vehicle_drive_PD(int output);
void vehicle_drive_blind(void);
void vehicle_reverse(void);
void vehicle_anticlockwise(int degree);
void vehicle_clockwise(int degree);
void servo_init(void);
void servo_pick(void);
void servo_drop(void);
void LED_init(void);
void LED_flash(unsigned int pin_num, unsigned int freq);
void LED_on(unsigned int pin_num);
void LED_off(unsigned int pin_num);

#endif
