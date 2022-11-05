#include "Sensors.h"

#define K_P_rot 90  //turning K_P
#define K_P_line 55 //straight line K_P
#define K_D 0

/*  Returns a PD value based on error calculated from linesensor sequence and gyroscope reading,
 *  if gyroscope reads the vehicle is truning around a corner, a greater K_P value is used.
 *  Parameters: None
 *  Returns: int output; An integer PD value that is passed into the PD drive function.
 */

int PD_control(void)
{
  double dt = 0.1;
  int error = 0, output = 0, D;
  static int previous_error, previous_sec = LINE, allow, count;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis, interval = 4000;

  //determining error
  if(linesensor[0] == 1 && linesensor[1] == 0 && linesensor[2] == 0) {error = 1;}
  else if((linesensor[0] == 0 && linesensor[1] == 1 && linesensor[2] == 0) || (linesensor[0] == 0 && linesensor[1] == 0 && linesensor[2] == 1)) {error = -1;}
  
  //Use gyroscope reading to determine using which K_P
  if(gyro_getreading() == TURN)
  {
    if(count == 0) {previousMillis = millis(); count++;}
    previous_sec = TURN;
    currentMillis = millis();
    D = (error - previous_error) / dt;
    output = int(K_P_rot) * error + int(K_D) * D;
  }

  else if(gyro_getreading() == LINE)
  {
    if(previous_sec == TURN)
    {
      if(currentMillis - previousMillis >= interval)
      {
        previousMillis = 0;
        allow = 1; //function is only able to switch back to line mode after timer reaches the interval value to ensure smooth turn
        count = 0;
      }
      
      if(allow == 1)
      {
        D = (error - previous_error) / dt;
        output = int(K_P_line) * error + int(K_D) * D;
      }

      else
      {
        D = (error - previous_error) / dt;
        output = int(K_P_rot) * error + int(K_D) * D;
      }
    }

    else if(previous_sec == LINE)
    {
      D = (error - previous_error) / dt;
      output = int(K_P_line) * error + int(K_D) * D;
    }
  }
  

  else
  {
    D = (error - previous_error) / dt;
    output = int(K_P_line) * error + int(K_D) * D;
  }

  previous_error = error;
  
  return output;
}
