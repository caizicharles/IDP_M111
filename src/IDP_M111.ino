#include "PD.h"
#include "Vehicle_Motion.h"

void setup() 
{
  Serial.begin(9600); 
  AFMS.begin();
  LED_init();
  servo_init();
  ultra_init();
  
  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() 
{ 
  set_mode();

  //Finite State Machine
  switch(mode) 
  {
    case 1:                 //drive_PD
    vehicle_drive_PD(PD_control()); 
    break;

    case 2:                 //drive_blind
    
    vehicle_drive_blind();
    break;

    case 3: //exit
    if(triple_count == 1)
    {
      vehicle_drive_blind();
      delay(700);
    }
    
    else if(triple_count == 2)
    {
      delay(600);
      vehicle_anticlockwise(90);
      triple_count = 0;
    }
    break;

    case 4:                 //pick up
    vehicle_stop();
    delay(650);
    hall_getreading();
    
    if(magnetic)
    {
      LED_on(RLED_pin);
      delay(5000);
    }

    else if(!magnetic)
    {
      LED_on(GLED_pin);
      delay(5000);
    }
    
    servo_pick();
    PICKED = true;
    delay(500);
    vehicle_anticlockwise(180);
    delay(500);
    break;

    case 5:                 //drop
    vehicle_drive_blind();
    delay(700);
    
    if(magnetic)
    {
      if(drop_count == 3)
      {
        vehicle_stop();
        delay(1000);
        vehicle_clockwise(90);
        delay(10);
        vehicle_drive_blind();
        delay(4200);
        vehicle_stop();
        delay(1000);
        servo_drop();
        vehicle_reverse();
        delay(1000);
        PICKED = false;
        drop_count = 0;
        triple_count = 0;
        DROPPED = true;
        vehicle_anticlockwise(180);
        vehicle_reverse();
        delay(400);

        /*
        vehicle_clockwise(90);
        delay(5);
        vehicle_drive_blind();
        delay(7000);
        vehicle_clockwise(90);
        delay(5);
        vehicle_stop();
        delay(500);
        while(1);*/
      }
    }

    else if(!magnetic)
    {
      if(drop_count == 1)
      {
        vehicle_stop();
        delay(1000);
        vehicle_clockwise(90);
        delay(10);
        vehicle_drive_blind();
        delay(4200);
        vehicle_stop();
        delay(1000);
        servo_drop();
        vehicle_reverse();
        delay(1000);
        DROPPED = true;
        PICKED = false;
        drop_count = 0;
        triple_count = 0;
        vehicle_anticlockwise(180);
        delay(5);
        vehicle_reverse();
        delay(400);

        /*
        vehicle_anticlockwise(90);
        delay(5);
        vehicle_drive_blind();
        delay(7500);
        vehicle_anticlockwise(90);
        delay(5);
        vehicle_stop();
        while(1);*/
      }
    }
    break;

    case 6:                 //align
    vehicle_stop();
    delay(500);

    if(touch == 1) {vehicle_clockwise(30);}
    else if(touch == 2) {vehicle_anticlockwise(30);}
    
    break;

    case 7: //back

    if(magnetic)
    {
      vehicle_stop();
      delay(200);
      vehicle_anticlockwise(1);
      delay(5);
      vehicle_drive_blind();
      delay(8200);
      vehicle_stop();
      delay(200);
      vehicle_clockwise(30);
      delay(5);
      servo_pick();
      while(1);
    }

    else if(!magnetic)
    {
      vehicle_drive_blind();
      delay(2500);
      vehicle_clockwise(90);
      delay(5);
      vehicle_drive_blind();
      delay(200);
    }
    
    triple_count = 0;
    PICKED = true;
    break;

    case 8:               //home
    vehicle_drive_blind();
    delay(500);
    vehicle_stop();
    delay(500);

    /*if(magnetic)
    {
      vehicle_anticlockwise(90);
      delay(5);
    }*/

    if(!magnetic)
    {
      vehicle_clockwise(90);
      delay(5);
    }

    vehicle_drive_blind();
    delay(4000);
    servo_pick();
    vehicle_stop();
    while(1);
    break;

    default: 
    vehicle_stop();
    break;
  }
}
