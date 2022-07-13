# balancingRobot
A self balancing robot, utilizing a Teensy LC.

This robot realized as a project in my mechatronics class. The design and code are heavily guided by the lecturers of my university.

This robot balances itself by reading it's pitch angle from an IMU (MPU6050) and moving the motors accordingly. To achieve this a PID-Control algorithm is used. For further stability rotary encoders meassure the rotation of the wheels. A second PD-Control loop ensures the robot stays in it's lateral position. For easy debugging while the robot is moving a bluetooth module is used, which transfers data wirelessly to the serial monitor of the Arduino IDE.

Parts list:
  
  mainboard (designed by the lecturers of my mechatronics class)
  
  IMU (MPU6050)
  
  Teensy LC
  
  Bluetooth module (HC-06)
  
  Motordriver (DRV8835)
  
  DC/DC converter
  
  2 LiFePo batteries
  
  Geared motor (V-Tec 6V Micro DC Motor)
