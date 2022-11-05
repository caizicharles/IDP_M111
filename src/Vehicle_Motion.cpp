#include "Sensors.h"
#include "Vehicle_Motion.h"

//Motor maximum and minimum values
#define motor_max 255
#define motor_min 0
#define corr 0

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motor_left = AFMS.getMotor(1);
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);

Servo Servo_1;

int triple_count, drop_count, mode, previous_mode;
unsigned int touch;
int motorval_left, motorval_right, prev_motorval_left, prev_motorval_right;
int ledState = LOW;
bool PICKED = false; 
bool DROPPED = false;

//mode0: inactive; mode1: drive_PD; mode2: drive_blind; mode3: exit; mode4: pick-up; mode5: drop-off; mode6: align; mode7: return; mode8: home;

/*  Sets the current mode of the vehicle based on various sensor readings
 * 
 *  Parameters: None
 *  Return: None
 */

void set_mode(void)
{
  unsigned int i, distance;
  static int previous_linesensor[3] = {0, 0, 0};
  static unsigned long previousMillis = 0;
  unsigned long currentMillis, interval = 1450;

  //Gets necessary sensor readings
  linesensor_getreading();
  distance = ultra_getreading();
  touch = touch_getreading();

  //drive_PD
  if((triple_count == 0) && ((linesensor[0] == 1 && linesensor[1] == 0 && linesensor[2] == 0)||(linesensor[0] == 0 && linesensor[1] == 1 && linesensor[2] == 0) ||(linesensor[0] == 0 && linesensor[1] == 0 && linesensor[2] == 1)))
  {
    delay(5);
    mode = 1;
  }

  //drive_blind
  else if(linesensor[0] == 0 && linesensor[1] == 0 && linesensor[2] == 0)
  {
    if(touch) {delay(5); mode = 6;}
    
    else {mode = 2;}
  }

  else if((linesensor[0] == 1 && linesensor[1] == 1 && linesensor[2] == 1) && PICKED == false)
  {
    if((previous_linesensor[0] == previous_linesensor[1] == previous_linesensor[2] == 0) || (previous_linesensor[0] == previous_linesensor[2] == 0 && previous_linesensor[1] == 1) ||
    (previous_linesensor[1] == previous_linesensor[2] == 0 && previous_linesensor[0] == 1) || (previous_linesensor[0] == previous_linesensor[1] == 0 && previous_linesensor[2] == 1))
    {
      triple_count++;
    }

    if((triple_count == 1) && (DROPPED == false)) //leaving the white box
    {
      delay(5);
      mode = 3;
      previousMillis = millis();
    }
    
    else if((triple_count == 2) && (DROPPED = false)) {delay(5); mode = 3;} //rotate anticlockwise after leaving the white box

    else if(triple_count == 1 && DROPPED == true) //leaves the red/green box to return to the white box
    {
      delay(5);
      mode = 7;
      //Serial.println("mode = 7");
      //Serial.println(PICKED);
    }
  }

  //drop
  else if(linesensor[0] == 0 && linesensor[1] == 1 && linesensor[2] == 1 && PICKED == true)
  {
    if(((previous_mode == 1) || (previous_mode == 2)) && (DROPPED == false)) //going into the red/green box
    {
      delay(5);
      mode = 5;
      drop_count++;
      if(drop_count == 4) {drop_count = 1;}
    }

    else if(((previous_mode == 1) || (previous_mode == 2)) && (DROPPED == true)) //return to the white box after leaving the red/green box
    {
      delay(5);
      mode = 8;
      drop_count = 1;
    }
  }

  //pick-up
  if(PICKED == false)
  {
    if((distance <= 4) && (previous_mode == 1 || previous_mode == 2)) //picks up the block
    {
      delay(5);
      mode = 4;
      delay(200);
    }
  }

  if((triple_count == 1) && (DROPPED == false)) //timer to ensure the vehicle leaves the white box, highest priority when PICKED == false
  {
    currentMillis = millis();

    if((currentMillis - previousMillis) >= interval)
    {
      delay(5);
      triple_count = 2;
      mode = 3;
    }
  }
  
  for(i = 0; i < 3; i++) 
  {
    if(previous_linesensor[i] != linesensor[i]) {previous_linesensor[i] = linesensor[i];}
  }

  if(previous_mode != mode) {previous_mode = mode;}

  //Serial.print("mode:  ");
  //Serial.println(mode);
}

/*  Stops the vehicle
 * 
 *  Parameters: None
 *  Return: None
 */

void vehicle_stop(void)
{
  LED_off(YLED_pin);
  
  motorval_left = 0;
  motorval_right = 0;

  if(motorval_left != prev_motorval_left) //preventing setting motors to the same value improves algorithm speed
  {
    motor_left->setSpeed(0);
    motor_left->run(FORWARD);
    prev_motorval_left = 0;
  }
  
  if(motorval_right != prev_motorval_right) //preventing setting motors to the same value improves algorithm speed
  {
    motor_right->setSpeed(0);
    motor_right->run(FORWARD);
    prev_motorval_right = 0;
  }
}

/*  Allows the vehicle to follow the line and corrects using PD algorithm
 * 
 *  Parameters: int output (output value returned from PD_control function)
 *  Return: None
 */
 
void vehicle_drive_PD(int output)
{
  LED_flash(YLED_pin, FLASH_freq); //amber light flashing
  
  motorval_left = 105 - output;
  motorval_right = 105 + output;

  //clamping
  if((motorval_left >= motor_max) || ((motorval_right >= motor_max))) {output = motor_max;}
  else if((motorval_left <= motor_min) || ((motorval_right <= motor_min))) {output = motor_min;}

  if(motorval_left != prev_motorval_left)
  {
    motor_left->setSpeed(motorval_left);
    motor_left->run(FORWARD);

    prev_motorval_left = motorval_left;
  }
  
  if(motorval_right != prev_motorval_right)
  {
    motor_right->setSpeed(motorval_right);
    motor_right->run(FORWARD);

    prev_motorval_right = motorval_right;
  }
}

/*  Drives straight
 * 
 *  Parameters: None
 *  Return: None
 */
 
void vehicle_drive_blind(void)
{
  LED_flash(YLED_pin, FLASH_freq); //amber light flashing

  motorval_left = 170;
  motorval_right = 170 + corr; //corr balances the left and right motor, tuned everyday
  
  if(motorval_left != prev_motorval_left)
  {
    motor_left->setSpeed(motorval_left);
    motor_left->run(FORWARD);

    prev_motorval_left = motorval_left;
  }
  
  if(motorval_right != prev_motorval_right)
  {
    motor_right->setSpeed(motorval_right);
    motor_right->run(FORWARD);

    prev_motorval_right = motorval_right;
  }
}

/*  Reversees the vehicle
 * 
 *  Parameters: None
 *  Return: None
 */

void vehicle_reverse(void)
{
  LED_flash(YLED_pin, FLASH_freq);
  motorval_left = 190;
  motorval_right = 190 + corr;
  
  if(motorval_left != prev_motorval_left)
  {
    motor_left->setSpeed(motorval_left);
    motor_left->run(BACKWARD);

    prev_motorval_left = motorval_left;
  }
  
  if(motorval_right != prev_motorval_right)
  {
    motor_right->setSpeed(motorval_right);
    motor_right->run(BACKWARD);

    prev_motorval_right = motorval_right;
  }
}

/*  Vehicle rotates anticlockwise
 * 
 *  Parameters: int degree (degrees that user wants to rotate)
 *  Return: None
 */

void vehicle_anticlockwise(int degree)
{
  unsigned int i = 0;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis, interval;

  LED_flash(YLED_pin, FLASH_freq);

  if(degree == 90) {interval = 1000;}
  else if(degree == 180) {interval = 2000;}
  else if(degree == 30) {interval = 290;}
  else if(degree == 1) {interval = 1230;} //customized degree
  else {interval == 2000; Serial.println("Invalid degree entered!");}

  if(i == 0) {previousMillis = millis(); i++;}
  motor_left->setSpeed(255);
  motor_left->run(BACKWARD);
  motor_right->setSpeed(255);
  motor_right->run(FORWARD);
  currentMillis = millis();

  prev_motorval_left = 255;
  prev_motorval_right = 255;

  while((currentMillis - previousMillis) < interval) //uses timer to rotate, linesensors are too close to the wheels restricts using linesensor to stop rotation
  {
    currentMillis = millis();

    //linesensor_getreading();
    //if(linesensor[0] == 1 && linesensor[1] == 0 && linesensor[2] == 0) {delay(400); break;}
  }

  vehicle_stop();
  delay(1000);
}

/*  Vehicle rotates clockwise
 * 
 *  Parameters: int degree (degrees that user wants to rotate)
 *  Return: None
 */
 
void vehicle_clockwise(int degree)
{
  unsigned int i = 0;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis, interval;

  LED_flash(YLED_pin, FLASH_freq);

  if(degree == 90) {interval = 1000;}
  else if(degree == 180) {interval = 2000;}
  else if(degree == 30) {interval = 290;}
  else if(degree == 1) {interval = 1500;} //customized degree
  else {interval == 2000; Serial.println("Invalid degree entered!");}

  if(i == 0) {previousMillis = millis(); i++;}
  motor_left->setSpeed(255);
  motor_left->run(FORWARD);
  motor_right->setSpeed(255);
  motor_right->run(BACKWARD);
  currentMillis = millis();

  prev_motorval_left = 255;
  prev_motorval_right = 255;

  while((currentMillis - previousMillis) < interval) //uses timer to rotate, linesensors are too close to the wheels restricts using linesensor to stop rotation
  {
    currentMillis = millis();
    //linesensor_getreading();
    //if(linesensor[0] == 1 && linesensor[1] == 0 && linesensor[2] == 0) {delay(400); break;}
  }

  vehicle_stop();
  delay(1000);
}

/*  Initializes servo
 * 
 *  Parameters: None
 *  Return: None
 */

void servo_init(void)
{
  Servo_1.attach(Servo_pin);
  servo_drop();
  delay(2000);
}

/*  Servo grabs
 * 
 *  Parameters: None
 *  Return: None
 */

void servo_pick(void)
{
  Servo_1.writeMicroseconds(1480);
}

/*  Servo releases
 * 
 *  Parameters: None
 *  Return: None
 */
 
void servo_drop(void)
{
  Servo_1.writeMicroseconds(1000);
}

/*  Initializes LEDs
 * 
 *  Parameters: None
 *  Return: None
 */
 
void LED_init(void)
{
  pinMode(GLED_pin, OUTPUT);
  pinMode(YLED_pin, OUTPUT);
  pinMode(RLED_pin, OUTPUT);
}

/*  Allows LED to flash at the set frequency
 * 
 *  Parameters: unsigned int pin_num (which LED pin is flashing); unsigned int freq (flash at what frequency)
 *  Return: None
 */

void LED_flash(unsigned int pin_num, unsigned int freq)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis(), interval;

  interval = 1000/freq;

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    if (ledState == LOW) {ledState = HIGH;} 
    
    else {ledState = LOW;}

    digitalWrite(pin_num, ledState);
  }
}

/*  Turns on a LED
 * 
 *  Parameters: unsigned int pin_num (turns which LED pin on)
 *  Return: None
 */

void LED_on(unsigned int pin_num)
{
  digitalWrite(pin_num, HIGH);
}

/*  Turns off a LED
 * 
 *  Parameters: unsigned int pin_num (turns which LED pin off)
 *  Return: None
 */

void LED_off(unsigned int pin_num)
{
  digitalWrite(pin_num, LOW);
}
