#include "protocol.h"

static uint8_t protocol_counter=1;
const char *CPROTOCOL="PROTOCOL";


#if ALIOLI_PROTOCOL_ALIGNMENT_TEST
void protocol_alignment_test() {
    while (1) {
        Serial.print("Size of Environment: ");
        Serial.println(sizeof(Environment));
        Serial.print("  > Size of Acelerometer: ");
        Serial.println(sizeof(Acelerometer));
        Serial.print("  > Size of WaterAnalisys: ");
        Serial.println(sizeof(WaterAnalisys));
        Serial.print("Size of Heartbeat: ");
        Serial.println(sizeof(HeartBeat));
        Serial.print("Size of Heartbeat: ");
        Serial.println(sizeof(UserRequest));
        Serial.println();
        delay(10000);
    }
}
#else
void protocol_alignment_test() {}
#endif

#if ALIOLI_PROTOCOL_DEBUG
const char * protocol_package_kind(uint8_t kind) {
    if (kind == ALIOLI_PROTOCOL_KIND_HEARTBEAT) {
        return "Heartbeat";
    } else if (kind == ALIOLI_PROTOCOL_KIND_ENVIRONMENT) {
        return "Environment";
    } else if (kind == ALIOLI_PROTOCOL_KIND_USERREQUEST) {
        return "UserRequest";
    } else {
        return NULL;
    }
}

void protocol_print_heartbeat(HeartBeat *heartbeat) {
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Request: %s%d", COLOR_WHITE, heartbeat->requested);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Answered: %s%d", COLOR_WHITE, heartbeat->answered);
}

void protocol_print_environment(Environment *environment) {
    char s1[20]="";
#ifdef ARDUINO_ARCH_AVR
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Altitude: %s%s", COLOR_WHITE, dtostrf(environment->altitude, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Pressure: %s%s", COLOR_WHITE, dtostrf(environment->pressure, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature Gy: %s%s", COLOR_WHITE, dtostrf(environment->temperaturegy, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 1: %s%s", COLOR_WHITE, dtostrf(environment->temperature1, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 2: %s%s", COLOR_WHITE, dtostrf(environment->temperature2, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature BMP: %s%s", COLOR_WHITE, dtostrf(environment->temperaturebmp, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Voltage: %s%s", COLOR_WHITE, dtostrf(environment->voltage, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Amperage: %s%s", COLOR_WHITE, dtostrf(environment->amperage, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Tmp: %s%d", COLOR_WHITE, environment->acelerometer.Tmp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Roll: %s%s", COLOR_WHITE, dtostrf(environment->acelerometer.roll, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Pitch: %s%s", COLOR_WHITE, dtostrf(environment->acelerometer.pitch, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Aceleromter Yaw: %s%s", COLOR_WHITE, dtostrf(environment->acelerometer.yaw, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH: %s%s", COLOR_WHITE, dtostrf(environment->analisys.ph, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH Temp: %s%s", COLOR_WHITE, dtostrf(environment->analisys.ph_temp, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP: %s%s", COLOR_WHITE, dtostrf(environment->analisys.orp, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP Temp: %s%s", COLOR_WHITE, dtostrf(environment->analisys.orp_temp, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Conductivity: %s%s", COLOR_WHITE, dtostrf(environment->analisys.conductivity, 8, 4, s1));
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Turbidity: %s%s", COLOR_WHITE, dtostrf(environment->analisys.turbidity, 8, 4, s1));
#else
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Altitude: %s%.2f", COLOR_WHITE, environment->altitude);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Pressure: %s%.2f", COLOR_WHITE, environment->pressure);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature Gy: %s%.2f", COLOR_WHITE, environment->temperaturegy);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 1: %s%.2f", COLOR_WHITE, environment->temperature1);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 2: %s%.2f", COLOR_WHITE, environment->temperature2);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature BMP: %s%.2f", COLOR_WHITE, environment->temperaturebmp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Voltage: %s%.2f", COLOR_WHITE, environment->voltage);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Amperage: %s%.2f", COLOR_WHITE, environment->amperage);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Tmp: %s%d", COLOR_WHITE, environment->acelerometer.Tmp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Roll: %s%.2f", COLOR_WHITE, environment->acelerometer.roll);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Pitch: %s%.2f", COLOR_WHITE, environment->acelerometer.pitch);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Aceleromter Yaw: %s%.2f", COLOR_WHITE, environment->acelerometer.yaw);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH: %s%.2f", COLOR_WHITE, environment->analisys.ph);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH Temp: %s%.2f", COLOR_WHITE, environment->analisys.ph_temp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP: %s%.2f", COLOR_WHITE, environment->analisys.orp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP Temp: %s%.2f", COLOR_WHITE, environment->analisys.orp_temp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Conductivity: %s%.2f", COLOR_WHITE, environment->analisys.conductivity);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Turbidity: %s%.2f", COLOR_WHITE, environment->analisys.turbidity);
#endif
}

void protocol_print_userrequest(UserRequest *userrequest) {
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> X: %s%d", COLOR_WHITE, userrequest->x);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Y: %s%d", COLOR_WHITE, userrequest->y);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Z: %s%d", COLOR_WHITE, userrequest->z);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> R: %s%d", COLOR_WHITE, userrequest->r);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Buttons 1: %s%u", COLOR_WHITE, userrequest->buttons1);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Buttons 2: %s%u", COLOR_WHITE, userrequest->buttons2);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Extension: %s%u", COLOR_WHITE, userrequest->extension);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Pitch: %s%d", COLOR_WHITE, userrequest->pitch);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Roll: %s%d", COLOR_WHITE, userrequest->roll);
}

void protocol_package_print(AlioliProtocol *package) {
    const char *kind = protocol_package_kind(package->kind);
    HeartBeat heartbeat;
    UserRequest userrequest;
    Environment environment;

    if (kind) {
        print_debug(CPROTOCOL, stdout, CBLUE, 0, "Package: %sKIND=%s%s%s - %sPAYLOAD_SIZE=%s%u%s", COLOR_CYAN, COLOR_WHITE, protocol_package_kind(package->kind), COLOR_CYAN, COLOR_CYAN, COLOR_WHITE, package->payload_size, CCYAN);

        if (package->kind == ALIOLI_PROTOCOL_KIND_HEARTBEAT) {
            memcpy(&heartbeat, package->payload, sizeof(HeartBeat));
            protocol_print_heartbeat(&heartbeat);
        } else if (package->kind == ALIOLI_PROTOCOL_KIND_ENVIRONMENT) {
            memcpy(&environment, package->payload, sizeof(Environment));
            protocol_print_environment(&environment);
        } else if (package->kind == ALIOLI_PROTOCOL_KIND_USERREQUEST) {
            memcpy(&userrequest, package->payload, sizeof(UserRequest));
            protocol_print_userrequest(&userrequest);
        } else {
            print_debug(CPROTOCOL, stdout, CRED, 0, "--- Programming ERROR: unknown kind detected at %s:%d!  ---", __FILE__, __LINE__);
        }
    } else {
        print_debug(CPROTOCOL, stdout, CBLUE, 0, "Package: %sKIND=%s%s%s - %sPAYLOAD_SIZE=%s%u%s", COLOR_CYAN, COLOR_RED, "Unknown", COLOR_CYAN, COLOR_CYAN, COLOR_WHITE, package->payload_size, COLOR_CYAN);
    }

}
#endif

uint16_t protocol_kind_size(uint8_t kind) {
    if (kind == ALIOLI_PROTOCOL_KIND_HEARTBEAT) {
        return sizeof(HeartBeat);
    } else if (kind == ALIOLI_PROTOCOL_KIND_ENVIRONMENT) {
        return sizeof(Environment);
    } else if (kind == ALIOLI_PROTOCOL_KIND_USERREQUEST) {
        return sizeof(UserRequest);
    } else {
        // Programming error, missing kind here!
        return 0;
    }
}

// Pack an Alioli Package to be sent
unsigned short int protocol_pack(AlioliProtocol *package, char **answer, size_t *answer_size, size_t *answer_allocated) {
    size_t temp_answer_size=0, until_payload=0, expected=*answer_size + package->payload_size + ALIOLI_PROTOCOL_SIZE_BASE;

    // Make sure it includes Alioli Protocol Header
    package->header = ALIOLI_PROTOCOL_MAGIC_HEADER;

    // Get memory for the package (+1 byte for counter+kind-crc)
    strcat_prealloc(answer, answer_allocated, *answer_size + ALIOLI_PROTOCOL_SIZE_BASE + package->payload_size + 1, __FILE__, __LINE__);
    if (*answer) {

        // Header + Kind + Payload Size
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->header), ALIOLI_PROTOCOL_SIZE_HEADER, __FILE__, __LINE__);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->counter), ALIOLI_PROTOCOL_SIZE_COUNTER, __FILE__, __LINE__);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->kind), ALIOLI_PROTOCOL_SIZE_KIND, __FILE__, __LINE__);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->payload_size), ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE, __FILE__, __LINE__);

        // Payload
        until_payload = *answer_size;
        if (package->payload_size) {
            strcat_realloc(answer, answer_size, answer_allocated, (char*) package->payload, package->payload_size, __FILE__, __LINE__);
        }
        temp_answer_size = *answer_size;

        // Copy 2 bytes of counter+kind to the string so it will be calculated together with the CRC8
        strcat_realloc(answer, &temp_answer_size, answer_allocated, (char*) &(package->counter), 1, __FILE__, __LINE__);
        strcat_realloc(answer, &temp_answer_size, answer_allocated, (char*) &(package->kind), 1, __FILE__, __LINE__);

        // Calculate CRC
#if ALIOLI_PROTOCOL_DEBUG_DEEP
        print_debug("PACK", stdout, CPURPLE, 0, "PACK-CRC8 - Counter: %u - Payload: %u", package->counter, package->payload_size);
        print_ashex(*answer, temp_answer_size, stdout);
        print_ashex(*answer + until_payload, temp_answer_size-until_payload, stdout);
#endif
        package->crc = CRC8((byte*) (*answer + until_payload), temp_answer_size-until_payload);

        // CRC (overwrite the last bytes from the string)
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->crc), ALIOLI_PROTOCOL_SIZE_CRC, __FILE__, __LINE__);

#if ALIOLI_ASSERTS
        // Assertion
        if (*answer_size!=expected) {
            // Stuck here, this is a programming error and never should happen
            while (1) {
                print_debug("PACK", stderr, CRED, 0, "PACK: Programming error: answer_size:%u!=%u:expected", *answer_size, ALIOLI_PROTOCOL_SIZE_BASE+package->payload_size);
                print_ashex((const char *) *answer, *answer_size, stdout);
                delay(5000);
            }
        }
#endif

    }

#if ALIOLI_PROTOCOL_DEBUG_DEEP
    print_debug("PACK", stdout, CBLUE, 0, "PACK: Counter: %u - Kind: %u - Payload: %u", package->counter, package->kind, package->payload_size);
    print_ashex((const char*) *answer, *answer_size, stdout);
#endif
#if ALIOLI_PROTOCOL_DEBUG
    protocol_package_print(package);
#endif

    return 1;
}


// Pack structures to bytes
unsigned short int protocol_new_package(uint8_t counter, uint8_t kind, byte *payload, char **answer, size_t *answer_size, size_t *answer_allocated) {
    AlioliProtocol package;
    package.header = ALIOLI_PROTOCOL_MAGIC_HEADER;
    package.counter = counter;
    package.kind = kind;
    package.payload_size = protocol_kind_size(kind);
    package.payload = payload;
    package.crc = 0;
    return protocol_pack(&package, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_heartbeat(HeartBeat *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_HEARTBEAT, (byte*) data, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_environment(Environment *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_ENVIRONMENT, (byte*) data, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_userrequest(UserRequest *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_USERREQUEST, (byte*) data, answer, answer_size, answer_allocated);
}


// Pack structures to bytes
unsigned short int protocol_unpack(AlioliProtocol *package, byte *data, uint8_t kind) {
    uint16_t dsize=protocol_kind_size(kind);
    if (dsize) {
        if (package->kind==kind) {

            // Dump payload on object
            if (package->payload_size==dsize) {
                if (dsize) {
                    memcpy(data, package->payload, package->payload_size);
                }
                return 1;
            } else {
#if OPTIMIZE
                Serial.println(F("Package payload size differs from function calculated size"));
#else
                print_debug(CPROTOCOL, stderr, CRED, 0, "Payload Size:%u != %u:size - Kind was: %u", package->payload_size, dsize, kind);
#endif
            }
        } else {
            // Different kind of package
#if OPTIMIZE
            Serial.println(F("Package kind differs from function kind"));
#else
            print_debug(CPROTOCOL, stderr, CRED, 0, "Package Kind: %u!=%u Function Kind", package->kind, kind);
#endif
        }
    } else {
#if OPTIMIZE
        Serial.println(F("Payload size: not found, unknown kind"));
#else
        print_debug(CPROTOCOL, stderr, CRED, 0, "Payload size: not found, unknow package kind %u", kind);
#endif
    }
    return 0;
}
unsigned short int protocol_unpack_heartbeat(AlioliProtocol *package, HeartBeat *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_HEARTBEAT);
}
unsigned short int protocol_unpack_environment(AlioliProtocol *package, Environment *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_ENVIRONMENT);
}
unsigned short int protocol_unpack_userrequest(AlioliProtocol *package, UserRequest *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_USERREQUEST);
}

// Setup functions
void protocol_setup_environment(Environment *environment) {
    environment->altitude=0.0;
    environment->pressure=0.0;
    environment->temperaturegy=0.0;
    environment->temperature1=0.0;
    environment->temperature2=0.0;
    environment->temperaturebmp=0.0;
    environment->voltage=0.0;
    environment->amperage=0.0;
    environment->acelerometer.Tmp=0;
    // environment->acelerometer.w=0;
    // environment->acelerometer.x=0;
    // environment->acelerometer.y=0;
    // environment->acelerometer.z=0;
    environment->acelerometer.roll=0;
    environment->acelerometer.pitch=0;
    environment->acelerometer.yaw=0;
    environment->acelerometer.roll_speed=0;
    environment->acelerometer.pitch_speed=0;
    environment->acelerometer.yaw_speed=0;
    environment->analisys.ph=0;
    environment->analisys.ph_temp=0;
    environment->analisys.orp=0;
    environment->analisys.orp_temp=0;
    environment->analisys.conductivity=0;
    environment->analisys.turbidity=0;
}

void protocol_setup_userrequest(UserRequest *userrequest) {
    userrequest->x = 0;
    userrequest->y = 0;
    userrequest->z = 0;
    userrequest->r = 0;
    userrequest->buttons1 = 0;
    userrequest->buttons2 = 0;
    userrequest->extension = 0;
    userrequest->pitch = 0;
    userrequest->roll = 0;
}

void protocol_setup_package(AlioliProtocol *package) {
    package->header = 0;
    package->kind = 0;
    package->payload_size = 0;
    package->payload = NULL;
    package->crc = 0;
}
void protocol_setup_status(AlioliProtocolStatus *status) {
    status->status = 0;
    status->total = 0;
}

// Parse byte-by-byte an Alioli Package
unsigned short int protocol_parse_char(byte element, AlioliProtocol *package, AlioliProtocolStatus *status) {
    byte crc=0;
    uint16_t version = ALIOLI_PROTOCOL_MAGIC_HEADER;
    unsigned short int done=0;

#if ALIOLI_PROTOCOL_DEBUG_DEEP
    print_debug("PARSE_CHAR", stdout, CCYAN, 0, "Total: %u - Status: %d - Char: %02X", status->total, status->status, element);
#endif

    // Prase
    if (
              (status->total==ALIOLI_PROTOCOL_READING_NODATA)
           && (status->status==ALIOLI_PROTOCOL_READING_NODATA)
           && (element == *((byte*) &version))
        ) {

        // Copy data
        memcpy(&(package->header), &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if (
              (status->total==ALIOLI_PROTOCOL_READING_HEADER_2)
           && (status->status==ALIOLI_PROTOCOL_READING_HEADER_2)
           && (element == *((byte*) (&version)+1))
        ) {

        // Copy data
        memcpy((&(package->header))+1, &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==ALIOLI_PROTOCOL_READING_COUNTER) && (status->status==ALIOLI_PROTOCOL_READING_COUNTER)) {

        // Copy data
        memcpy((&(package->counter)), &element, 1);
        status->total++;
#if ALIOLI_PROTOCOL_DEBUG_DEEP
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Counter: %u", package->counter);
#endif

        // Update status
        status->status++;

    } else if ((status->total==ALIOLI_PROTOCOL_READING_KIND) && (status->status==ALIOLI_PROTOCOL_READING_KIND)) {

        // Copy data
        memcpy(&(package->kind), &element, 1);
        status->total++;
#if ALIOLI_PROTOCOL_DEBUG_DEEP
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Kind: %u", package->kind);
#endif

        // Update status
        status->status++;

    } else if ((status->total==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_1) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_1)) {

        // Copy data
        memcpy(&(package->payload_size), &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_2) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_2)) {

        // Copy data
        memcpy((&(package->payload_size))+1, &element, 1);
        status->total++;

        // Verify
        if (package->payload_size<=ALIOLI_PROTOCOL_MAX_SIZE) {

            // Extra byte is for CRC verification purpose +1 byte for kind on CRC8 calculus
            package->payload = (byte*) calloc(package->payload_size+2, sizeof(byte));
#if ALIOLI_PROTOCOL_DEBUG_DEEP
            print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Payload: %u", package->payload_size);
#endif

            // Update status
            status->status++;

        } else {

            // Package too big - Broken package - Clear status
            status->status = ALIOLI_PROTOCOL_READING_NODATA;

        }

    } else if ((status->total>=ALIOLI_PROTOCOL_READING_PAYLOAD) && (((status->total)-ALIOLI_PROTOCOL_READING_PAYLOAD)<(package->payload_size)) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD)) {

        // Copy data
        memcpy((package->payload)+(status->total)-ALIOLI_PROTOCOL_READING_PAYLOAD, &element, 1);
        status->total++;
#if ALIOLI_PROTOCOL_DEBUG_DEEP
        print_ashex((char*) package->payload, status->total-ALIOLI_PROTOCOL_READING_PAYLOAD, stdout);
#endif

        // Update status if we got all expected bytes
        if (((status->total)-ALIOLI_PROTOCOL_READING_PAYLOAD) == (package->payload_size)) {
            status->status++;
        }

    } else if ((status->total==ALIOLI_PROTOCOL_READING_PAYLOAD+package->payload_size) && (status->status==ALIOLI_PROTOCOL_READING_CRC)) {

        // Copy data
        memcpy(&(package->crc), &element, 1);
        status->total++;

        // Copy counter + kind to the end of the string
        memcpy(package->payload + package->payload_size, &(package->counter), 1);
        memcpy(package->payload + package->payload_size + 1, &(package->kind), 1);

        // Create CRC
#if ALIOLI_PROTOCOL_DEBUG_DEEP
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Total: %u - Payload: %u", status->total, package->payload_size);
        print_debug("PARSE_CHAR", stdout, CPURPLE, 0, "PARSE-CRC8 - Counter: %u - Payload: %u", package->counter, package->payload_size);
        print_ashex((char*) package->payload, package->payload_size+2, stdout);
#endif
        crc = CRC8((const byte *) package->payload, package->payload_size+2);

        // Verify CRC
        if (crc==package->crc) {

            // Wipeout last character from the string with '\0'
            package->payload[package->payload_size] = 0;

            // Set we are done
            done = 1;

        } else {
            // CRC didn't verify - Broken package - Clear status
            print_debug("PARSE_CHAR", stdout, CRED, 0, "CRC FAILED: package:%02X != %02X:calculated", package->crc, crc);
            status->status = ALIOLI_PROTOCOL_READING_NODATA;
        }

    } else {
        print_debug("PARSE_CHAR", stdout, CRED, 0, "BROKEN");

        // Broken package - Clear status
        status->status = ALIOLI_PROTOCOL_READING_NODATA;

    }

    // Clean package if empty
    if (status->status==ALIOLI_PROTOCOL_READING_NODATA) {

        // Empty memory if some is reserved
        if (package->payload) {
            free(package->payload);
        }

        // Reset package
        package->header = 0;
        package->counter = 0;
        package->kind = 0;
        package->payload_size = 0;
        package->payload = NULL;
        package->crc = 0;

        // Reset total bytes processed
        status->total = 0;

    } else if (done) {
        // Update status and total
        status->status = ALIOLI_PROTOCOL_READING_NODATA;
        status->total = 0;
    }

#if ALIOLI_PROTOCOL_DEBUG_DEEP
    print_debug("PARSE_CHAR", stdout, CWHITE, 0, "status:%u - return: %u - kind: %u - payload: %u\n", status->status, done, package->kind, package->payload_size);
#endif

    // Return if we finished a package
    return done;
}

