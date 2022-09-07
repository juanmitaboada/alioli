#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "buoyancy.ino"
// #if PAD
// #include "pad.ino"
// #endif

const char *CS="CS";
const char *CL="CL";

void control_setup(long int now) {
#if DEBUG_CONTROL
    print_debug(CS, stdout, "purple", COLOR_NORMAL, "INI");
#endif

// #if PAD
//     // Set pad
//     pad_setup(now);
// #endif

    // Set other controls
    buoyancy_setup(now);

#if DEBUG_CONTROL
    print_debug(CS, stdout, "purple", COLOR_NORMAL, "DONE");
#endif
}

// === LOOP === ===========================================================

void control_loop(long int now) {
// #if PAD
// #if DEBUG_CONTROL
//     print_debug(CL, stdout, "purple", COLOR_NORMAL, "Pad");
// #endif
//     pad_loop(now);
// #endif

#if DEBUG_CONTROL
    print_debug(CL, stdout, "purple", COLOR_NORMAL, "Buoyancy");
#endif
    buoyancy_loop(now);
}
