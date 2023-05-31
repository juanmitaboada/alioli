# Some commands to move arround [PlatformIO](https://platformio.org/)

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

# Permission issues with Raspberry Pi Pico

For Raspberry Pi Pico there is a HIDDEN permission issue that prevents PICO from being flashed properly, execute this:

    sudo echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="2e8a", MODE:="0666"' >  /etc/udev/rules.d/99-platformio-udev.rules

# Empty Flash in Raspberry Pi Pico
Drop flash_nuke.uf2 onto Raspberry Pi Pico when started up with BOOTSEL pushed and join to the host system as a mass storage
