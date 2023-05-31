# alioli
Alioli UAV ROV Submarine Drone Software Framework for Arduino

More information and details at:

https://www.juanmitaboada.com/alioli-rov-submarine-drone-software-framework-for-arduino/

## Description:

- The entire **Alioli ROV** code is developed in **C programming language** using [platformio](https://platformio.org/) (check [the link provided before to my website to get into more details](https://www.juanmitaboada.com/alioli-rov-submarine-drone-software-framework-for-arduino/))
- The only folders that Alioli ROV uses to do its jobs are:
  - **buoy:** software used for buoy communication with QGroundControl (Mavlink) and connection to the network.
  - **common:** common functions for buoy and rov
  - **rov:** software used for the rov to manage commands from buoy and internal control and telemetry
- **testio:** this is a test folder with a simple example to test whether you have [platformio](https://platformio.org/) software under control or not. I don't recommend working with the rest of the software until you have control of [platformio](https://platformio.org/) since this is the first issue you may have.
- **NOTE:** *requirements.txt* is part of the deprecated dome software

## Quick steps

I have done this project using [platformio](https://platformio.org/). Here you have some steps to bring up the project quickly:

**Initialize the project:**

    pio project init --board megaatmega2560 --board esp32dev --board pico

**Compile only for MEGA 2560:**

    pio run -e megaatmega2560

**Compile and upload to MEGA 2560:**

    pio run -e megaatmega2560 -t upload

**Compile only for Raspberry Pi Pico:**

    pio run -e pico

**Compile and upload to Raspberry Pi Pico:**

    pio run -e pico -t upload

