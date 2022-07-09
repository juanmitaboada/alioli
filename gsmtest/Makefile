# makeEspArduino
CHIP = esp32
# BOARD = esp32
# INCLUDE_VARIANT = uPesy_esp32_wroom_devkit
USB_PORT = /dev/ttyUSB0
UPLOAD_PORT = $(USB_PORT)
MONITOR_COM = screen $(USB_PORT) 115200
ESP_ROOT = /opt/esp32
SKETCH=sketchbook/
LIBS=/opt/esp32/libraries/
BUILD_DIR=build
EXPAND_LIBS = 1
include ../../frameworks/makeEspArduino/makeEspArduino.mk

# EspXArduino
#ARDUINO_ARCH = esp32
#ARDUINO_VARIANT = esp32
#SERIAL_PORT = /dev/ttyUSB0
## uncomment and set the right serail baud according to your sketch (default to 115200)
##SERIAL_BAUD = 115200
## uncomment this to use the 1M SPIFFS mapping
##SPIFFS_SIZE = 1
##ESP8266_VERSION=git
## uncomment to eclude this lib from dependencies
##EXCLUDE_USER_LIBS=ESP8266TrueRandom
## uncomment this to use the 1M SPIFFS mapping
##SPIFFS_SIZE = 1
##ESP8266_VERSION=git
## uncomment to exclude this lib from dependencies
##USER_DEFINE = -D_SSID_=\"YourSSID\" -D_WIFI_PASSWORD_=\"YourPassword\"
##OTA_IP = 192.168.1.184
##OTA_PORT = 8266 
##OTA_AUTH = password
#include ../../frameworks/Esp8266-Arduino-Makefile/espXArduino.mk
#monitor:
#	screen $(SERIAL_PORT)

full:
	make
	make upload
	make monitor
