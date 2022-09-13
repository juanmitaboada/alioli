#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "brain.h"

BrainConfig brain_config;
const char *BS="BS";
const char *BL="BL";

// === SETUP === ==========================================================

void brain_setup(long int now) {
#if DEBUG_BRAIN
#if OPTIMIZE
    Serial.print(F("BS: INI "));
#else
    print_debug(BS, stdout, "purple", COLOR_NORMAL, "INI");
#endif
#endif

    // Initilize
    brain_config.nextevent=0;

#if DEBUG_BRAIN
#if OPTIMIZE
    Serial.println(F("DONE"));
#else
    print_debug(BS, stdout, "purple", COLOR_NORMAL, "DONE");
#endif
#endif
}

// === LOOP === ===========================================================

void brain_loop(long int now) {
#if DEBUG_BRAIN
    char senstemp[20] = "";
#endif

    // Check next event
    if (brain_config.nextevent<now) {

        // Update nextevent
        brain_config.nextevent = now+BRAIN_LOOKUP_MS;

#if DEBUG_BRAIN
        // print_debug("BRAIN-LOOP", stdout, "yellow", COLOR_NORMAL, "Brain");
        // print_debug(BS, stdout, "white", COLOR_NORMAL, "Ac: (%d, %d, %d) - Gy: (%d, %d, %d) - TempGy:%s Temp1:%s Temp2:%s", rov.pos.AcX, rov.pos.AcY, rov.pos.AcZ, rov.pos.GyX, rov.pos.GyY, rov.pos.GyZ, dtostrf(rov.environment.temperaturegy, 8, 4, senstemp), dtostrf(rov.environment.temperature1, 8, 4, senstemp), dtostrf(rov.environment.temperature2, 8, 4, senstemp));
        print_debug(BL, stdout, "white", COLOR_NORMAL, "(%3d, %3d)", rov.pos.angx, rov.pos.angy);
#endif
    }
}
