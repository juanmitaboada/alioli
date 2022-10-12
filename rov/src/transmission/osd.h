#ifndef OSD_H
#define OSD_H

#include "lib/constants.h"
#include "mavlink/alioli/mavlink.h"

struct TOSDConfig {
    long int nextevent;
};
typedef struct TOSDConfig OSDConfig;

void osd_setup(long int now);
void osd_loop(long int now);

#endif
