#include "protocol.h"

static uint8_t protocol_counter=1;
const char *CPROTOCOL="PROTOCOL";

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
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Altitude: %s%.2f", COLOR_WHITE, environment->altitude);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Pressure: %s%.2f", COLOR_WHITE, environment->pressure);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature Gy: %s%.2f", COLOR_WHITE, environment->temperaturegy);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 1: %s%.2f", COLOR_WHITE, environment->temperature1);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature 2: %s%.2f", COLOR_WHITE, environment->temperature2);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Temperature BMP: %s%.2f", COLOR_WHITE, environment->temperaturebmp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Voltage: %s%.2f", COLOR_WHITE, environment->voltage);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Amperage: %s%.2f", COLOR_WHITE, environment->amperage);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer Tmp: %s%d", COLOR_WHITE, environment->acelerometer.Tmp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer AngX: %s%.2f", COLOR_WHITE, environment->acelerometer.angx);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Acelerometer AngY: %s%.2f", COLOR_WHITE, environment->acelerometer.angy);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Aceleromter AngZ: %s%.2f", COLOR_WHITE, environment->acelerometer.angz);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH: %s%.2f", COLOR_WHITE, environment->analisys.ph);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys PH Temp: %s%.2f", COLOR_WHITE, environment->analisys.ph_temp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP: %s%.2f", COLOR_WHITE, environment->analisys.orp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys ORP Temp: %s%.2f", COLOR_WHITE, environment->analisys.orp_temp);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Conductivity: %s%.2f", COLOR_WHITE, environment->analisys.conductivity);
    print_debug(CPROTOCOL, stdout, CCYAN, COLOR_NOHEAD, "> Analisys Turbidity: %s%.2f", COLOR_WHITE, environment->analisys.turbidity);
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

// Pack an Alioli Package to be sent
unsigned short int protocol_pack(AlioliProtocol *package, char **answer, size_t *answer_size, size_t *answer_allocated) {
    size_t temp_answer_size=0, until_payload=0, expected=*answer_size + package->payload_size + ALIOLI_PROTOCOL_SIZE_BASE;

    // Make sure it includes Alioli Protocol Header
    package->header = ALIOLI_PROTOCOL_MAGIC_HEADER;

    // Get memory for the package (+1 byte for counter+kind-crc)
    strcat_prealloc(answer, answer_allocated, *answer_size + ALIOLI_PROTOCOL_SIZE_BASE + package->payload_size + 1, __FILE__, __LINE__);
    if (*answer) {

        // Header + Kind + Payload Size
        print_ashex(*answer, *answer_size, stdout);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->header), ALIOLI_PROTOCOL_SIZE_HEADER, __FILE__, __LINE__);
        print_ashex(*answer, *answer_size, stdout);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->counter), ALIOLI_PROTOCOL_SIZE_COUNTER, __FILE__, __LINE__);
        print_ashex(*answer, *answer_size, stdout);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->kind), ALIOLI_PROTOCOL_SIZE_KIND, __FILE__, __LINE__);
        print_ashex(*answer, *answer_size, stdout);
        strcat_realloc(answer, answer_size, answer_allocated, (char*) &(package->payload_size), ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE, __FILE__, __LINE__);
        print_ashex(*answer, *answer_size, stdout);
        // Payload
        until_payload = *answer_size;
        if (package->payload_size) {
            strcat_realloc(answer, answer_size, answer_allocated, (char*) package->payload, package->payload_size, __FILE__, __LINE__);
            print_ashex(*answer, *answer_size, stdout);
        }
        temp_answer_size = *answer_size;

        // Copy 2 bytes of counter+kind to the string so it will be calculated together with the CRC8
        strcat_realloc(answer, &temp_answer_size, answer_allocated, (char*) &(package->counter), 1, __FILE__, __LINE__);
        strcat_realloc(answer, &temp_answer_size, answer_allocated, (char*) &(package->kind), 1, __FILE__, __LINE__);

        // Calculate CRC
        print_ashex(*answer, temp_answer_size, stdout);
        print_debug("PACK", stdout, CPURPLE, 0, "PACK-CRC8 - Counter: %u - Payload: %u", package->counter, package->payload_size);
        print_ashex(*answer, temp_answer_size, stdout);
        print_ashex(*answer + until_payload, temp_answer_size-until_payload, stdout);
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

    print_debug("PACK", stdout, CBLUE, 0, "PACK: Counter: %u - Kind: %u - Payload: %u", package->counter, package->kind, package->payload_size);
    print_ashex((const char*) *answer, *answer_size, stdout);
    // protocol_package_print(package);

    return 1;
}


// Pack structures to bytes
unsigned short int protocol_new_package(uint8_t counter, uint8_t kind, uint16_t payload_size, byte *payload, char **answer, size_t *answer_size, size_t *answer_allocated) {
    AlioliProtocol package;
    package.header = ALIOLI_PROTOCOL_MAGIC_HEADER;
    package.counter = counter;
    package.kind = kind;
    package.payload_size = payload_size;
    package.payload = payload;
    package.crc = 0;
    return protocol_pack(&package, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_heartbeat(HeartBeat *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_HEARTBEAT, sizeof(HeartBeat), (byte*) &data, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_environment(Environment *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_ENVIRONMENT, sizeof(Environment), (byte*) data, answer, answer_size, answer_allocated);
}
unsigned short int protocol_pack_userrequest(UserRequest *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated) {
    return protocol_new_package(counter, ALIOLI_PROTOCOL_KIND_USERREQUEST, sizeof(UserRequest), (byte*) data, answer, answer_size, answer_allocated);
}


// Pack structures to bytes
unsigned short int protocol_unpack(AlioliProtocol *package, byte *data, uint8_t kind) {
    uint16_t dsize=0;
    if (package->kind==kind) {
        if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_HEARTBEAT)
            )  {
            dsize = sizeof(HeartBeat);
        } else if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_ENVIRONMENT)
            ) {
            dsize = sizeof(Environment);
        } else if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_USERREQUEST)
            ) {
            dsize = sizeof(UserRequest);
        } else {
            // Programming error, missing kind here!
            return 0;
        }

        // Dump payload on object
        if (package->payload_size==dsize) {
            if (dsize) {
                memcpy(data, package->payload, package->payload_size);
            }
            return 1;
        } else {
            print_debug(CPROTOCOL, stderr, CRED, 0, "Payload Size:%u != %u:size - Kind was: %u", package->payload_size, dsize, kind);
            return 0;
        }
    } else {
        // Different kind of package
        print_debug(CPROTOCOL, stderr, CRED, 0, "Package Kind: %u!=%u Function Kind", package->kind, kind);
        return 0;
    }
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
    environment->acelerometer.angx=0;
    environment->acelerometer.angy=0;
    environment->acelerometer.angz=0;
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

    print_debug("PARSE_CHAR", stdout, CCYAN, 0, "Total: %u - Status: %d - Char: %02X", status->total, status->status, element);
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
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Counter: %u", package->counter);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==ALIOLI_PROTOCOL_READING_KIND) && (status->status==ALIOLI_PROTOCOL_READING_KIND)) {

        // Copy data
        memcpy(&(package->kind), &element, 1);
        status->total++;
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Kind: %u", package->kind);

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
            print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Payload: %u", package->payload_size);

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
        print_ashex((char*) package->payload, status->total-ALIOLI_PROTOCOL_READING_PAYLOAD, stdout);

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
        print_debug("PARSE_CHAR", stdout, CYELLOW, 0, "Total: %u - Payload: %u", status->total, package->payload_size);
        print_debug("PARSE_CHAR", stdout, CPURPLE, 0, "PARSE-CRC8 - Counter: %u - Payload: %u", package->counter, package->payload_size);
        print_ashex((char*) package->payload, package->payload_size+2, stdout);
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

    print_debug("PARSE_CHAR", stdout, CWHITE, 0, "status:%u - return: %u - kind: %u - payload: %u\n", status->status, done, package->kind, package->payload_size);

    // Return if we finished a package
    return done;
}

