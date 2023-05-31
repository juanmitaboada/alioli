# alioli
Alioli UAV ROV Submarine Drone Software Framework for Arduino

More information and details at:

https://www.juanmitaboada.com/alioli-rov-submarine-drone-software-framework-for-arduino/

## Description:

- The entire **Alioli ROV** code is developed in **C programming language** using [PlatformIO](https://platformio.org/) (check [the link provided before to my website to get into more details](https://www.juanmitaboada.com/alioli-rov-submarine-drone-software-framework-for-arduino/))
- The only folders that Alioli ROV uses to do its jobs are:
  - **buoy:** software used for buoy communication with QGroundControl (Mavlink) and connection to the network.
  - **common:** common functions for buoy and rov
  - **rov:** software used for the rov to manage commands from buoy and internal control and telemetry
- **testio:** this is a test folder with a simple example to test whether you have [PlatformIO](https://platformio.org/) software under control or not. I don't recommend working with the rest of the software until you have control of [PlatformIO](https://platformio.org/) since this is the first issue you may have.
- **NOTE:** *requirements.txt* is part of the deprecated dome software

## Quick steps

I have done this project using [PlatformIO](https://platformio.org/). Here you have some steps to bring up the project quickly:

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

## Permission issues with Raspberry Pi Pico

For Raspberry Pi Pico there is a HIDDEN permission issue that prevents PICO from being flashed properly, execute this:

    sudo echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", MODE:="0666"' >  /etc/udev/rules.d/99-platformio-udev.rules

## Empty Flash in Raspberry Pi Pico

Drop flash_nuke.uf2 onto Raspberry Pi Pico when started up with BOOTSEL pushed and join to the host system as a mass storage
