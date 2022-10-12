#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "osd.h"

OSDConfig osd_config;

mavlink_system_t mavlink_system = {
    OSD_MAVLINK_SYSID, // System ID (1-255)
    OSD_MAVLINK_COMPID  // Component ID (a MAV_COMPONENT value)
};

const char *OSD="OSD";

// === SETUP === ===============================================================================
void osd_setup(long int now) {

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("OSD: INI: "));
#else
    print_debug(OSD, stdout, CPURPLE, COLOR_NORMAL, "INI");
#endif
#endif

    // Set local config
    osd_config.nextevent=0;

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("DONE"));
#else
    print_debug(OSD, stdout, CPURPLE, COLOR_NORMAL, "DONE");
#endif
#endif
}

int8_t battery_percent() {
    int8_t i=0;
    float voltages[] = BATTERY_VOLTAGES;
    for (i=10; i>0; i--) {
        if (rov.environment.voltage>=voltages[i]) {
            break;
        }
    }
    return i*10;
}

// === LOOP === ================================================================================

void osd_loop(long int now) {
    mavlink_message_t mavlink_msg;
    char answer[MAVLINK_MAX_PAYLOAD_LEN]={0};
    unsigned int answer_size=0;

    // Check temperature lookup
    if (osd_config.nextevent<now) {

        // Set next event
        osd_config.nextevent = now+OSD_LOOKUP_MS;

        // === HEARTBEAT ===
        mavlink_msg_heartbeat_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            MAV_TYPE_SUBMARINE,         // Type = Submarine
            MAV_AUTOPILOT_INVALID,      // Autopilot = Invalid (not a flight controller)
            MAV_MODE_GUIDED_ARMED,      // Base mode = Armed
            0,                          // Custom mode = None
            MAV_STATE_ACTIVE            // System status = Active
        );
        answer_size = (unsigned int) mavlink_msg_to_send_buffer((uint8_t*) answer, &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);

        // === STATUS ===
        mavlink_msg_sys_status_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            0,              // Sensors present
            0,              // Sensors enabled
            0,              // Sensors health
            999,            // Load %
            (uint16_t) (rov.environment.voltage*1000),  // Battery voltage (mV)
            (uint16_t) rov.environment.amperage,        // Battery current (cA)
            battery_percent(),                          // Battery remaining (%)
            0,              // Communication drop rate (UART, I2C, SPI, CAN)
            0,              // Communication errors (UART, I2C, SPI, CAN)
            0,              // Errors count 1
            0,              // Errors count 2
            0,              // Errors count 3
            0               // Errors count 4
        );
        answer_size = (unsigned int) mavlink_msg_to_send_buffer((uint8_t*) answer, &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);

        // === ATTITUDE ===
        mavlink_msg_attitude_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            millis(),       // Time since system boot (millis)
            rov.environment.acelerometer.roll,  // ROLL angle [rad] (-pi..+pi)
            rov.environment.acelerometer.pitch, // PITCH angle [rad] (-pi..+pi)
            rov.environment.acelerometer.yaw,   // YAW angle [rad] (-pi..+pi)
            0.0,                                // ROLL angular speed [rad/s]
            0.0,                                // PITCH angular speed [rad/s]
            0.0                                 // YAW angular speed [rad/s]
        );
        /*
        mavlink_msg_attitude_quaternion_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            millis(),       // Time since system boot (millis)
            rov.environment.acelerometer.w,     // Quaternion 1, w
            rov.environment.acelerometer.x,     // Quaternion 2, x
            rov.environment.acelerometer.y,     // Quaternion 3, y
            rov.environment.acelerometer.z,     // Quaternion 4, z
            0.0,                                // ROLL angular speed [rad/s]
            0.0,                                // PITCH angular speed [rad/s]
            0.0                                 // YAW angular speed [rad/s]
        );
        */
        answer_size = (unsigned int) mavlink_msg_to_send_buffer((uint8_t*) answer, &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);

        // GPS Status to remove "LOW SATS" message
        mavlink_msg_gps_raw_int_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            GPS_FIX_TYPE_NO_GPS,    // GPS fixed type
            0,                      // Timestamp [us]
            0,                      // Latitude [degE7]
            0,                      // Longitude [degE7]
            0,                      // Altitude [mm]
            UINT16_MAX,             // EPH
            UINT16_MAX,             // EPV
            0,                      // GPS ground speed [cm/s]
            UINT16_MAX,             // Course over ground speed [cdeg]
            20                      // Number of satellites visible
        );
        answer_size = (unsigned int) mavlink_msg_to_send_buffer((uint8_t*) answer, &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);

        // RADIO Status
        mavlink_msg_rc_channels_pack(
            mavlink_system.sysid,
            mavlink_system.compid,
            &mavlink_msg,
            millis(),   // Time since system boot (millis)
            0,          // Total number of RC channels being received
            0,          // Channel 1 value
            0,          // Channel 2 value
            0,          // Channel 3 value
            0,          // Channel 4 value
            0,          // Channel 5 value
            0,          // Channel 6 value
            0,          // Channel 7 value
            0,          // Channel 8 value
            0,          // Channel 9 value
            0,          // Channel 10 value
            0,          // Channel 11 value
            0,          // Channel 12 value
            0,          // Channel 13 value
            0,          // Channel 14 value
            0,          // Channel 15 value
            0,          // Channel 16 value
            0,          // Channel 17 value
            0,          // Channel 18 value
            254         // RSSI: Received signal strength [0-254], UINT8_MAX: invalid/unknown
        );
        answer_size = (unsigned int) mavlink_msg_to_send_buffer((uint8_t*) answer, &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);


#if DEBUG_OSD
        print_debug(OSD, stdout, CYELLOW, COLOR_NORMAL, "Batt:%d - (r:%d, p:%d, y:%d)", (int) battery_percent(), rov.environment.acelerometer.roll, rov.environment.acelerometer.pitch, rov.environment.acelerometer.yaw);
#endif
    }

}
