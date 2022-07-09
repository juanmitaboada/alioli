# Configuraiton updated to Arduino 1.8.15 & lates Arduino-Makefile

BOARD_TAG     = mega
BOARD_SUB     = atmega2560
ARDUINO_DIR   = /opt/arduino-1.8.15
DEVICE_PATH   = /dev/ttyACM0

include ../../frameworks/Arduino-Makefile/Arduino.mk

full:
	-echo "#ifndef VERSION_H" > lib/version.h
	-echo "#define VERSION_H" >> lib/version.h
	-echo >> lib/version.h
	-echo "#define BUILD_DATE \"$(shell date)\"" >> lib/version.h
	-echo "#define BUILD_VERSION \"$(shell date +"%Y%m%d%H%M%S%Z")\"" >> lib/version.h
	-echo >> lib/version.h
	-echo "#endif" >> lib/version.h
	make
	make upload
	make monitor
