IDP_M111
**Software Documentation**

To implement a line tracking vehicle that can determine whether if a block is magnet and deliver that block to the appropriate location, I coded a finite state machine that takes sensor readings (Sensors.cpp and Sensors.h), sets a customized mode, calls actuator functions (Vehicle_Motion.cppand Vehicle_Motion.h), and implement a sequence of actions (IDP_M111.ino).

**It is very important to know what each mode entails:**

Mode0: inactive; <br>
Mode1: drive_PD; <br>
Mode2: drive_blind; <br>
Mode3: exit; <br>
Mode4: pick-up; <br>
Mode5: drop-off; <br>
Mode6: align; <br>
Mode7: return; <br>
Mode8: home. <br>

Redundancies are also included to ensure successful execution of sequences. For instance, timers are set on top of getting line sensor readings to set Mode 3 taht leaves the initial white box. These precautions reduces the algorithm's dependency on sensors thus rendering the vehicle less susceptable to environmental conditions. Libraries were used to make the program easily readable.

A software day starts with calibrating the vehicle: make sure no wires are loose; tune the corr variable (Vehicle_Motion.cpp) to correct for motor imbalance; adjust K_P value for both line and turn and alter the threshold value for gyroscope (Sensors.cpp) since motor output power varies each day. All changes made are then tested and debugged afterwards. Overall I spent over 90 hours on software, 60% of those time are dedicated to calibration.

**All the code is included in the src folder。**
