# alioli
Alioli UAV ROV Submarine Drone Software Framework for Arduino

More information and details at:

https://www.juanmitaboada.com/alioli-rov-submarine-drone-software-framework-for-arduino/

## NOTES:

- The entire Alioli ROV code is developed in C programming language using https://platformio.org/ (check the link provided before)
- The only folders that Alioli ROV uses to do its jobs are:
  - **buoy:** software used for buoy communication with QGroundControl (Mavlink) and connection to the network.
  - **common:** common functions for buoy and rov
  - **rov:** software used for the rov to manage commands from buoy and internal control and telemetry
- requirements.txt is part of the deprecated dome software
