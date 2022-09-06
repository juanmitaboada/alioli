#include "protocol.h"

// Pack an Alioli Package to be sent
byte* protocol_pack(AlioliProtocol *package) {
    byte *answer = NULL;

    // Make sure it includes Alioli Protocol Header
    package->header = ALIOLI_PROTOCOL_MAGIC_HEADER;

    // Get memory for the package
    answer = (byte*) malloc(sizeof(byte) * (ALIOLI_PROTOCOL_SIZE_BASE + package->payload_size));
    if (answer) {

        // Header + Kind + Payload Size
        memcpy(answer, package, ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE);
        // Payload
        if (package->payload_size) {
            memcpy(answer + ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE, package->payload, package->payload_size);
        }

        // Copy 1 byte of kind to the string so it will be calculated together with the CRC8
        memcpy(answer + ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE + package->payload_size, &(package->kind), 1);

        // Calculate CRC
        package->crc = CRC8((const byte *) answer, ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE + package->payload_size + 1);

        // CRC (overwrite the last bytes from the string)
        memcpy(answer + ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE + package->payload_size, &(package->crc), ALIOLI_PROTOCOL_SIZE_CRC);
    }

    return answer;
}


// Pack structures to bytes
byte* protocol_new_package(uint8_t kind, uint16_t payload_size, byte *payload) {
    AlioliProtocol package;
    package.header = ALIOLI_PROTOCOL_MAGIC_HEADER;
    package.kind = kind;
    package.payload_size = payload_size;
    package.payload = payload;
    package.crc = 0;
    return protocol_pack(&package);
}
byte* protocol_pack_heartbeat(HeartBeat *data) {
    return protocol_new_package(ALIOLI_PROTOCOL_KIND_HEARTBEAT, sizeof(HeartBeat), (byte*) &data);
}
byte* protocol_pack_status(ROVStatus *data) {
    return protocol_new_package(ALIOLI_PROTOCOL_KIND_STATUS, sizeof(ROVStatus), (byte*) data);
}
byte* protocol_pack_userrequest(UserRequest *data) {
    return protocol_new_package(ALIOLI_PROTOCOL_KIND_USERREQUEST, sizeof(UserRequest), (byte*) data);
}


// Parse byte-by-byte an Alioli Package
unsigned short int protocol_parse_char(byte element, AlioliProtocol *package, AlioliProtocolStatus *status) {
    byte crc=0;
    uint16_t version = ALIOLI_PROTOCOL_MAGIC_HEADER;

    if (
              (status->total==0)
           && (status->status==ALIOLI_PROTOCOL_READING_NODATA)
           && (element == *((byte*) &version))
        ) {

        // Copy data
        memcpy(&(package->header), &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if (
              (status->total==1)
           && (status->status==ALIOLI_PROTOCOL_READING_HEADER_2)
           && (element == *((byte*) (&version)+1))
        ) {

        // Copy data
        memcpy((&(package->header))+1, &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==2) && (status->status==ALIOLI_PROTOCOL_READING_KIND)) {

        // Copy data
        memcpy(&(package->kind), &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==3) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_1)) {

        // Copy data
        memcpy(&(package->payload_size), &element, 1);
        status->total++;

        // Update status
        status->status++;

    } else if ((status->total==4) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_2)) {

        // Copy data
        memcpy((&(package->payload_size))+1, &element, 1);
        status->total++;

        // Verify
        if (package->payload_size<=ALIOLI_PROTOCOL_MAX_SIZE) {

            // Extra byte is for CRC verification purpose +1 byte for kind on CRC8 calculus
            package->payload = (byte*) malloc(sizeof(byte)*package->payload_size+1);

            // Update status
            status->status++;

        } else {

            // Package too big - Broken package - Clear status
            status->status = ALIOLI_PROTOCOL_READING_NODATA;

        }

    } else if ((status->total>=5) && (((status->total)-5)<(package->payload_size)) && (status->status==ALIOLI_PROTOCOL_READING_PAYLOAD)) {

        // Copy data
        memcpy((package->payload)+(status->total)-5, &element, 1);
        status->total++;

        // Update status if we got all expected bytes
        if (((status->total)-5) == (package->payload_size)) {
            status->status++;
        }

    } else if ((status->total==5+package->payload_size) && (status->status==ALIOLI_PROTOCOL_READING_CRC)) {

        // Copy data
        memcpy(&(package->crc), &element, 1);
        status->total++;

        // Copy kind to the end of the string
        memcpy(package->payload+package->payload_size, &(package->kind), 1);

        // Create CRC
        crc = CRC8((const byte *) package->payload, package->payload_size+1);

        // Verify CRC
        if (crc==package->crc) {

            // Wipeout last character from the string with '\0'
            package->payload[package->payload_size] = 0;

            // Update status
            status->status = ALIOLI_PROTOCOL_READING_DONE;

        } else {
            // CRC didn't verify - Broken package - Clear status
            status->status = ALIOLI_PROTOCOL_READING_NODATA;
        }

    } else {

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
        package->kind = 0;
        package->payload_size = 0;
        package->payload = NULL;
        package->crc = 0;

        // Reset total bytes processed
        status->total = 0;

    }

    // Return if we finished a package
    return (status->status==ALIOLI_PROTOCOL_READING_DONE);
}

// Pack structures to bytes
unsigned short int protocol_unpack(AlioliProtocol *package, byte *data, uint8_t kind) {
    uint16_t size=0;
    if (package->kind==kind) {
        if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_HEARTBEAT)
               && (package->payload_size==sizeof(HeartBeat))
            )  {
            size = sizeof(HeartBeat);
        } else if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_STATUS)
               && (package->payload_size==sizeof(ROVStatus))
            ) {
            size = sizeof(ROVStatus);
        } else if (
                  (package->kind == ALIOLI_PROTOCOL_KIND_USERREQUEST)
               && (package->payload_size==sizeof(UserRequest))
            ) {
            size = sizeof(UserRequest);
        } else {
            // Programming error, missing kind here!
            return 0;
        }

        // Dump payload on object
        if (size) {
            memcpy(data, package->payload, package->payload_size);
        }
        return 1;
    } else {
        // Different kind of package
        return 0;
    }
}
unsigned short int protocol_unpack_heartbeat(AlioliProtocol *package, HeartBeat *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_HEARTBEAT);
}
unsigned short int protocol_unpack_status(AlioliProtocol *package, ROVStatus *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_STATUS);
}
unsigned short int protocol_unpack_userrequest(AlioliProtocol *package, UserRequest *data) {
    return protocol_unpack(package, (byte*) data, ALIOLI_PROTOCOL_KIND_USERREQUEST);
}


