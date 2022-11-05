#include <Arduino.h>
#include <Arduino_LSM6DS3.h>

//linesensor
#define DARK 0
#define LIGHT 1
//schimitt trigger
#define TH 350
#define TL 70
//gyroscope
#define TURN true
#define LINE false
//ultrasound
#define echoPin 5
#define trigPin 6
//hall effect sensor
#define hallpin A3
//reboot button
#define buttonpin 8
//touch sensor
#define touch_leftpin 12
#define touch_rightpin 13

unsigned int linesensor[3] = {};
bool magnetic;

/*  Converts analogue readings into digital readings
 * 
 *  Parameters: int reading (current analogue reading); int prev_state (previous digital state determined by the function)
 *  Return: int digital state
 */

int schmitt_trigger(int reading, int prev_state)
{
  if(reading >= int(TH))
  {
    return DARK;
  } 
  
  else if(reading <= int(TL))
  {
    return LIGHT;
  }
  
  else
  {
    return prev_state;
  }
}

/*  Prints linesensor readings for testing
 * 
 *  Parameters: None
 *  Return: None
 */

void linesensor_print(void)
{
  unsigned int i;
  
  Serial.print("L");
  for(i = 0; i < 3; i++)
  {
    Serial.print("   ");
    Serial.print(linesensor[i]);
    delay(2);
  }
  Serial.print("   ");
  Serial.println("R");
}

/*  Gets linesensor readings by accessing corresponding analogue pins
 * 
 *  Parameters: None
 *  Return: None
 */

void linesensor_getreading(void)
{
  double linesensor_analog[3] = {0, 0, 0};
  unsigned int i, st_val;

  linesensor_analog[0] = analogRead(A0);
  linesensor_analog[1] = analogRead(A1);
  linesensor_analog[2] = analogRead(A2);


  for(i = 0; i<3; i++)
  {
    st_val = schmitt_trigger(linesensor_analog[i], linesensor[i]);
    
    if(linesensor[i] != st_val)
    {
      linesensor[i] = st_val;
    }
  }

  //linesensor_print();
}

/*  Gets gyroscope reading based by accessing onboard IMU
 * 
 *  Parameters: None
 *  Return: bool state (determines whether the vehicle is following a curved or straight line)
 */

bool gyro_getreading(void)
{
  float gyro[3] = {}, sum_z = 0, avg_z = 0;
  double threshold = 12;
  unsigned int len = 11;

  for(unsigned int i = 0; i < len; i++)
  {
    if(IMU.gyroscopeAvailable()) {IMU.readGyroscope(gyro[0], gyro[1], gyro[2]);}
    /*
    Serial.print(gyro[0]);
    Serial.print("   ");
    Serial.print(gyro[1]);
    Serial.print("   ");
    Serial.println(gyro[2]);
    */
    sum_z += gyro[2];
  }
  
  avg_z = sum_z / len; //gyroscope is too sensitive, took average for better precision
  
  if(avg_z < 0) {avg_z = -avg_z;}
  
  //Serial.print("average:   ");
  //Serial.println(avg_z);
  
  if(avg_z >= threshold) {return TURN;}
  else {return LINE;}
}

/*  Initializes ultrasound by allocating pins
 * 
 *  Parameters: None
 *  Return: None
 */

void ultra_init(void)
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

/*  Gets ultrasound reading that is distance
 * 
 *  Parameters: None
 *  Return: unsigned int distance
 */

unsigned int ultra_getreading(void)
{
  long duration;
  int distance;

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");

  return distance;
}

/*  Gets hall effect sensor reading
 * 
 *  Parameters: None
 *  Return: None
 */

void hall_getreading(void)
{
  unsigned int val = analogRead(hallpin);

  if(val <= 200) {magnetic = true; Serial.println("magnetic");}
  else{magnetic = false; Serial.println("NOT magnetic");}
}

/*  Gets left and right touch sensor reading
 * 
 *  Parameters: None
 *  Return: unsigned int state (if left is triggered, state = 1; if right is triggered, state = 2; otherwise state = 0)
 */

unsigned int touch_getreading(void)
{
  unsigned int touch_left = digitalRead(touch_leftpin);
  unsigned int touch_right = digitalRead(touch_rightpin);

  if(!touch_left) {return 1;}
  else if(!touch_right) {return 2;}
  else {return 0;}
}
