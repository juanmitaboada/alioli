#include "communication.h"

CommunicationConfig communication_config;

const char *CCOMMUNICATION="COMMUNICATION";

void communication_setup() {
    communication_config.mavlink_buf = NULL;
    communication_config.mavlink_buf_size = 0;
    communication_config.mavlink_buf_allocated = 0;
}

void communication_reset() {
    communication_config.mavlink_buf_size = 0;
}
unsigned short int mavlink_msgcat(char **answer, unsigned int *answer_size, unsigned int *answer_allocated, mavlink_message_t *msg) {
    char temp[MAVLINK_MAX_PAYLOAD_LEN]={0};
    int len=0;
    len = mavlink_msg_to_send_buffer((uint8_t*) temp, msg);
    return strcat_realloc(answer, answer_size, answer_allocated, temp, len, __FILE__, __LINE__);
}

unsigned short int remote_msg(char **buf, unsigned int *buf_size, unsigned int *buf_allocated, char **answer, unsigned int *answer_size, unsigned int *answer_allocated) {
    mavlink_status_t status;
    mavlink_message_t msg;
    int chan = MAVLINK_COMM_0; // , len=0;
    unsigned short int gotmsg=0, bypass=0;
    unsigned int buf_idx=0;

    // Put all together
    strcat_realloc(&communication_config.mavlink_buf, &communication_config.mavlink_buf_size, &communication_config.mavlink_buf_allocated, *buf, *buf_size, __FILE__, __LINE__);
    // print_ashex(communication_config.mavlink_buf, communication_config.mavlink_buf_size, stderr);

    // While data in the buffer and not MAVLINK message detected, keep reading
    yield();
    while (buf_idx<communication_config.mavlink_buf_size) {

        // Parse MAVLINK mg
        if (mavlink_parse_char(chan, (uint8_t) communication_config.mavlink_buf[buf_idx], &msg, &status)) {

            // Say we got a MAVLINK message
            gotmsg=1;
            // ... DECODE THE MESSAGE PAYLOAD HERE ...
            // https://mavlink.io/kr/messages/common.html

            switch(msg.msgid) {
                case MAVLINK_MSG_ID_HEARTBEAT:
                    // bypass=1;
                    {
                        // save the sysid and compid of the received heartbeat from the ARDUPILOT for use in sending new messages
                        // uint8_t received_sysid = msg.sysid;
                        // uint8_t received_compid = msg.compid;
                        mavlink_heartbeat_t packet;
                        mavlink_msg_heartbeat_decode(&msg, &packet);

                        //sending the heartbeat
                        mavlink_msg_heartbeat_pack(mavlink_system.sysid, mavlink_system.compid, &msg, MAV_TYPE_SUBMARINE, MAV_AUTOPILOT_INVALID, MAV_MODE_GUIDED_ARMED, 0, MAV_STATE_ACTIVE);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);

                        // mavlink_msg_vfr_hud_pack(mavlink_system.sysid, mavlink_system.compid, &msg , 3, 2, 45, 1, 10, 1);
                        // mavlink_msgcat(answer, answer_size, answer_allocated, &msg);

                        // mavlink_msg_gps_global_origin_pack(mavlink_system.sysid, mavlink_system.compid, &msg , 29.4*pow(10,7), -13.51*pow(10, 7), 15, 1646238463);
                        // mavlink_msgcat(answer, answer_size, answer_allocated, &msg);

                        // GPS position
                        mavlink_msg_global_position_int_pack(mavlink_system.sysid, mavlink_system.compid, &msg, millis(), buoy.gps.latitude*pow(10,7), buoy.gps.longitude*pow(10, 7), buoy.gps.altitude, buoy.gps.altitude, 0, 0, 0, 45);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);
                        print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "GPS: %f - %f - %f", buoy.gps.latitude, buoy.gps.longitude, buoy.gps.altitude);

                        // Attitude
                        mavlink_msg_attitude_pack(mavlink_system.sysid, mavlink_system.compid, &msg, millis(), 45.0, 45.0, 45.0, 0.0, 0.0, 0.0);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);

                        // GPS_RAW_INT
                        // GPS_STATUS
                        // ATTITUDE
/*
                        mavlink_msg_vfr_hud_pack(received_sysid, received_compid, &msg , 3, 2, 45, 1, 10, 1);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);
                        mavlink_msg_gps_global_origin_pack(received_sysid, received_compid, &msg , 36.715831516305*pow(10,7), -4.19757573985523*pow(10, 7), 15, 1646238463);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);
                        mavlink_msg_global_position_int_pack(received_sysid, received_compid, &msg, millis(), 36.715831516305*pow(10,7), -4.19757573985523*pow(10, 7), 15, 15, 0, 0, 0, 45);
                        mavlink_msgcat(answer, answer_size, answer_allocated, &msg);
*/

                        //sending the ping
                        // mavlink_msg_ping_pack(mavlink_system.sysid, mavlink_system.compid, &msg, 93372036854775807ULL, 963497880, received_sysid, received_compid);
                        // len = mavlink_msg_to_send_buffer(buffer, &msg);
                        // sendport(len);
                    }
                    break;

                case MAVLINK_MSG_ID_COMMAND_LONG:
                    {
                        // print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "COMMAND_LONG: %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
                        // mavlink_msg_command_long_decode
                        if (mavlink_msg_command_long_get_param1(&msg)==MAV_CMD_REQUEST_MESSAGE) {
                            // print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "COMMAND_LONG: %f", mavlink_msg_command_long_get_param1(&msg));
                        } else {
                            print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "COMMAND_LONG: unknown CMD id %d", mavlink_msg_command_long_get_command(&msg));
                        }
                    }
                    break;

                case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
                    {
                        // uint8_t received_sysid = msg.sysid;
                        // uint8_t received_compid = msg.compid;
                        mavlink_param_request_list_t request;
                        mavlink_msg_param_request_list_decode(&msg, &request);
                        mavlink_param_union_t paramUnion;
                        // const char *param_id="heading";

                        paramUnion.param_float = 45.0;
                        paramUnion.type = MAV_PARAM_TYPE_REAL32;

                        print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "REQUEST LIST: %d, sequence: %d from component %d of system %d - Sending %f", msg.msgid, msg.seq, msg.compid, msg.sysid, paramUnion.param_float);

                        // mavlink_msg_param_set_pack(mavlink_system.sysid, mavlink_system.compid, &msg, msg.sysid, msg.compid, param_id, paramUnion.param_float, paramUnion.type);
                    }
                    break;

                default:
                    //Do nothing
                    print_debug(CCOMMUNICATION, stdout, CYELLOW, 0, "Unknown message with ID %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
                    break;
            }
        }

        // Visit next character
        buf_idx++;

    }
    yield();

    // If requested, bypass message
    (*buf_size) = 0;
    if (bypass) {
        strcat_realloc(buf, buf_size, buf_allocated, communication_config.mavlink_buf, buf_idx, __FILE__, __LINE__);
    }

    // Move back what is left to the beginning of the buffer
    if (buf_idx<communication_config.mavlink_buf_size) {
        // Left some bytes for reading in the future
        memmove(communication_config.mavlink_buf, communication_config.mavlink_buf+buf_idx,communication_config.mavlink_buf_size-buf_idx);
    } else {
        // Everything readen
        communication_config.mavlink_buf_size = 0;
    }

    // If something in the buffer, warn about it!
    if (communication_config.mavlink_buf_size) {
        print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "MAVLINK Buffer: %d", communication_config.mavlink_buf_size);
    }

    // We didn't get any MAVLINK message
    if (!gotmsg) {
        print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR: didn't get a MAVLINK message");
    }

    // Return result
    return gotmsg;
}

unsigned short int rov_msg(char *buf, unsigned int buf_size, char **answer, unsigned int *answer_size, unsigned int *answer_allocated) {
    mavlink_status_t status;
    mavlink_message_t msg;
    int chan = MAVLINK_COMM_0, len=0;
    unsigned short int gotmsg=0;
    char heartbeat[100]={0};
    unsigned int buf_idx=0;

    print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "GO");
    while (buf_idx<buf_size) {
        if (mavlink_parse_char(chan, (uint8_t) buf[buf_idx], &msg, &status)) {
            print_debug(CCOMMUNICATION, stdout, CCYAN, 0, "Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
            gotmsg=1;
            // ... DECODE THE MESSAGE PAYLOAD HERE ...
            // https://mavlink.io/kr/messages/common.html
            mavlink_msg_heartbeat_pack(mavlink_system.sysid, mavlink_system.compid, &msg, MAV_TYPE_SURFACE_BOAT, MAV_AUTOPILOT_INVALID, MAV_MODE_GUIDED_ARMED, 0, MAV_STATE_ACTIVE);
             // len = mavlink_msg_to_send_buffer((uint8 t*)buf, &message);
             len = mavlink_msg_to_send_buffer((uint8_t *) heartbeat, &msg);
            if (serial_send(RS485_SERIAL, heartbeat, len)) {
            }
            delay(100);
        }
        buf_idx++;
    }

    if (!gotmsg) {
        print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR: didn't get MAVLINK message");
    }
    delay(1000);

    return gotmsg;
}

/*
unsigned short int mavmsg_self(char *buf, unsigned int buf_size) {
    mavlink_status_t status;
    mavlink_message_t msg;
    int chan = MAVLINK_COMM_0, len=0;
    unsigned short int gotmsg=0, incomingByte=0;
    char heartbeat[100]={0};

    while (1) {
        print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "GO");
        while (serial_available(RS485_SERIAL)) {
            incomingByte = serial_read(RS485_SERIAL);
            if (incomingByte>=0) {
                if (mavlink_parse_char(chan, (uint8_t) incomingByte, &msg, &status)) {
                    print_debug(CCOMMUNICATION, stdout, CCYAN, 0, "Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
                    gotmsg=1;
                    // ... DECODE THE MESSAGE PAYLOAD HERE ...
                    // https://mavlink.io/kr/messages/common.html
                    mavlink_msg_heartbeat_pack(mavlink_system.sysid, mavlink_system.compid, &msg, MAV_TYPE_SURFACE_BOAT, MAV_AUTOPILOT_INVALID, MAV_MODE_GUIDED_ARMED, 0, MAV_STATE_ACTIVE);
                     // len = mavlink_msg_to_send_buffer((uint8 t*)buf, &message);
                     len = mavlink_msg_to_send_buffer((uint8_t *) heartbeat, &msg);
                    if (serial_send(RS485_SERIAL, heartbeat, len)) {
                    }
                    delay(100);
                }
            } else {
                print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR?");
            }
        }

        if (!gotmsg) {
            print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR: didn't get MAVLINK message");
        }
        delay(1000);
    }

    return gotmsg;
}
*/
