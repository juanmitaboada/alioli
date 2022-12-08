#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SYSNAME "Rov"
#define VERSION "1.0"

#define TIMEOUT_RS485_MS 0
#define TRANSFER_RS485_WAIT_MS 0
#define BATTERY_CURRENT 7.0   // Battery is 7.0Ah
// #define BATTERY_VOLTAGES {11.59, 11.63, 11.76, 11.87, 11.97, 12.07, 12.18, 12.29, 12.41, 12.53, 12.64} // LIPO?
// #define BATTERY_VOLTAGES {11.63, 11.70, 11.81, 11.96, 12.11, 12.23, 12.41, 12.51, 12.65, 12.78, 12.89} // LEAD ACID
#define BATTERY_LEAD_ACID {10.50, 11.31, 11.58, 11.75, 11.90, 12.06, 12.20, 12.32, 12.42, 12.50, 12.70}
#define BATTERY_WET {10.50, 11.31, 11.58, 11.75, 11.90, 12.06, 12.20, 12.32, 12.42, 12.50, 12.60}
#define BATTERY_AGM {10.50, 11.51, 11.66, 11.81, 11.95, 12.05, 12.15, 12.30, 12.50, 12.75, 13.00}
#define BATTERY_LITHIUM {10.00, 12.00, 12.90, 13.00, 13.10, 13.13, 13.16, 13.20, 13.28, 13.32, 13.60}
#define BATTERY_LITHIUM_AV {9.00, 10.25, 10.45, 10.60, 10.70, 10.80, 11.10, 11.35, 11.60, 12.10, 12.60}
#define BATTERY_LIFEPO {10.00, 12.00, 12.80, 12.90, 13.00, 13.1, 13.2, 13.3, 13.4, 13.6, 14.4}
#define BATTERY_VOLTAGES BATTERY_LITHIUM

#define OSD_MAVLINK_SYSID 1
#define OSD_MAVLINK_COMPID 1

#endif
