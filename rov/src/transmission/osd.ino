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

unsigned short int mavlink_msgcat(char **answer, unsigned int *answer_size, unsigned int *answer_allocated, mavlink_message_t *msg) {
    char temp[MAVLINK_MAX_PAYLOAD_LEN]={0};
    int len=0;
    len = mavlink_msg_to_send_buffer((uint8_t*) temp, msg);
    return strcat_realloc(answer, answer_size, answer_allocated, temp, len, __FILE__, __LINE__);
}

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

// === LOOP === ================================================================================

void osd_loop(long int now) {
    mavlink_message_t mavlink_msg;
    char fixed_answer[100]="";
    char *answer=fixed_answer;
    unsigned int answer_size=0, answer_allocated=100;

    // Check temperature lookup
    if (osd_config.nextevent<now) {

        // Set next event
        osd_config.nextevent = now+OSD_LOOKUP_MS;

        // STATUS
        mavlink_msg_sys_status_pack(mavlink_system.sysid, mavlink_system.compid, &mavlink_msg, 0, 0, 0, 50, (uint16_t) (rov.environment.voltage*1000), (uint16_t) (rov.environment.amperage*100), 100, 0, 0, 0, 0, 0, 0);
        mavlink_msgcat(&answer, &answer_size, &answer_allocated,                       &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);
        answer_size = 0;

        // Attach HEARTBEAT
        mavlink_msg_heartbeat_pack(mavlink_system.sysid, mavlink_system.compid,         &mavlink_msg, MAV_TYPE_SUBMARINE, MAV_AUTOPILOT_INVALID, MAV_MODE_GUIDED_ARMED, 0, MAV_STATE_ACTIVE);
        mavlink_msgcat(&answer, &answer_size, &answer_allocated,                       &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);
        answer_size = 0;

        // Attach BATTERY STATUS
        mavlink_msg_battery_status_pack(mavlink_system.sysid, mavlink_system.compid, &(mavlink_msg), 1, MAV_BATTERY_FUNCTION_ALL, MAV_BATTERY_TYPE_UNKNOWN, 20, (uint16_t) (rov.environment.voltage*1000), (uint16_t) rov.environment.amperage, 1, 1, 55);
        mavlink_msgcat(&answer, &answer_size, &answer_allocated,                       &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);
        answer_size = 0;

        // Attach ATTITUDE
        mavlink_msg_attitude_pack(mavlink_system.sysid, mavlink_system.compid, &mavlink_msg, millis(), rov.environment.acelerometer.roll, rov.environment.acelerometer.pitch, rov.environment.acelerometer.yaw, 0.0, 0.0, 0.0);
        mavlink_msgcat(&answer, &answer_size, &answer_allocated,                       &mavlink_msg);
        serial_send(OSD_SERIAL, answer, answer_size);
        answer_size = 0;

#if DEBUG_OSD
        print_debug(OSD, stdout, CYELLOW, COLOR_NORMAL, "OSD: %umV %uA - (r: %d, p:%d, y:%d)", (uint16_t) (rov.environment.voltage*1000), rov.environment.amperage, rov.environment.acelerometer.roll, rov.environment.acelerometer.pitch, rov.environment.acelerometer.yaw);
#endif
    }

}
