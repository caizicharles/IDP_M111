#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#define TURN true
#define LINE false

extern int linesensor[3];
extern bool magnetic;

void linesensor_getreading(void);
bool gyro_getreading(void);
void ultra_init(void);
unsigned int ultra_getreading(void);
void hall_getreading(void);
unsigned int touch_getreading(void);

#endif
