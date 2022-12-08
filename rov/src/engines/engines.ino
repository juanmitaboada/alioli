#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "monitor/monitor.h"
#include "engines/engines.h"

const char *ES="ES";

struct TEngineConfig {
    char name[20];
    unsigned short int pin_dir;
    unsigned short int pin_enable;
    unsigned short int dir;
    unsigned short int enable;
    unsigned short int cw;
};
typedef struct TEngineConfig EngineConfig;

struct TEnginesConfig {
    EngineConfig engines[6];
};
typedef struct TEnginesConfig EnginesConfig;
EnginesConfig engines_config;

// === FUNCTIONS === ======================================================

void engine_set(EngineConfig *engine, unsigned short int dir, unsigned short int gear) {
    unsigned short int positive=0;

    // Set GEAR
    if (gear==ENGINE_GEAR_STOP) {
        digitalWrite (engine->pin_enable, LOW);
        engine->enable = 0;
    }

    // Set DIRECTION
    if (dir) {

        // Positive
        positive = (dir == ENGINE_BLUE);

        // Reverse for CCW engines
        if (!engine->cw) {
            positive = !positive;
        }

        // Execute
        if (positive) {
            digitalWrite (engine->pin_dir, HIGH);
        } else {
            digitalWrite (engine->pin_dir, LOW);
        }

        // Remember direction
        engine->dir = dir;
    }
    if (gear!=ENGINE_GEAR_STOP) {
        digitalWrite (engine->pin_enable, HIGH);
        engine->enable = 1;
    }

#if DEBUG_ENGINES
    print_debug(ES, stdout, "blue", COLOR_NORMAL, "    %s - Move engine (Dir: %d:%d - Enable: %d:%d)", engine->name, engine->pin_dir, engine->dir, engine->pin_enable, engine->enable);
#endif

}

void engine_setup(const char* name, unsigned short int idx, unsigned short int cw, unsigned short int pin_dir, unsigned short int pin_enable) {
    EngineConfig *engine=NULL;

    // Set pinmode
#if DEBUG_ENGINES
    print_debug(ES, stdout, "blue", COLOR_NORMAL, "    %s - Seting up engine (Dir:%d - Enable:%d)", name, pin_dir, pin_enable);
#endif

    // Set up configuration
    engine = &(engines_config.engines[idx]);
    engine->pin_dir = pin_dir;
    engine->pin_enable = pin_enable;
    engine->cw = cw;
    sprintf(engine->name, name);

    // Configure port
    pinMode (engine->pin_enable, OUTPUT);
    pinMode (engine->pin_dir, OUTPUT);

#if DEBUG_ENGINES
    print_debug(ES, stdout, CGREEN, COLOR_NORMAL, "    %s - Ready", engine->name);
#endif

}


void engine_test(unsigned short int idx) {

        EngineConfig *engine = &(engines_config.engines[idx]);

        // Motor gira en un sentido
#if DEBUG_ENGINES
        print_debug(ES, stdout, CCYAN, COLOR_NORMAL, "    %s - Turn side 1", engine->name);
#endif

        // Go forward
        engine_set(engine, ENGINE_BLUE, ENGINE_GEAR_FULL);
        delay(1000);

        // Stop engine
        engine_set(engine, ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        delay(500);

        // Go backward
#if DEBUG_ENGINES
        print_debug(ES, stdout, CCYAN, COLOR_NORMAL, "    %s - Turn side 2", engine->name);
#endif
        engine_set(engine, ENGINE_RED, ENGINE_GEAR_FULL);
        delay(1000);

        // Motor no gira
        engine_set(engine, ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
}

void engine_move(const char* name, unsigned short int target) {

#if DEBUG_ENGINES
    print_debug(ES, stdout, CGREEN, COLOR_NORMAL, "    ENGINE MOVE - Start");
#endif

    // Set up configuration
    if (target==ROVER_MOVE_STOP) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
    } else if (target==ROVER_MOVE_UP) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
    } else if (target==ROVER_MOVE_DOWN) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
    } else if (target==ROVER_MOVE_FORWARD) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
    } else if (target==ROVER_MOVE_BACKWARD) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
    } else if (target==ROVER_MOVE_LEFT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
    } else if (target==ROVER_MOVE_RIGHT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
    } else if (target==ROVER_LOOK_LEFT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
    } else if (target==ROVER_LOOK_RIGHT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
    } else if (target==ROVER_LOOK_UP) {
#if OPTIMIZE
        Serial.println(F("Movemente not alowed: look up"));
#else
        print_debug("ENGINE-MOVE", stderr, CYELLOW, 0, "This movement is not allowed in this design!");
#endif
    } else if (target==ROVER_LOOK_DOWN) {
#if OPTIMIZE
        Serial.println(F("Movemente not alowed: look down"));
#else
        print_debug("ENGINE-MOVE", stderr, CYELLOW, 0, "This movement is not allowed in this design!");
#endif
    } else if (target==ROVER_ROLL_LEFT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
    } else if (target==ROVER_ROLL_RIGHT) {
        engine_set(&engines_config.engines[ENGINE_FRONT_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_FRONT_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_LATERAL_LEFT], ENGINE_BLUE, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_LATERAL_RIGHT], ENGINE_RED, ENGINE_GEAR_FULL);
        engine_set(&engines_config.engines[ENGINE_BACK_LEFT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
        engine_set(&engines_config.engines[ENGINE_BACK_RIGHT], ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
    } else {
#if OPTIMIZE
        Serial.println(F("Movemente not alowed: look down"));
#else
        print_debug("ENGINE-MOVE", stderr, CRED, 0, "Wrong movement!");
#endif
    }


/*
#if DEBUG_ENGINES
        print_debug(ES, stdout, CCYAN, COLOR_NORMAL, "    %s - Turn side 1", engine->name);
#endif
        digitalWrite (engine->pin_enable, HIGH);
        digitalWrite (engine->pin_dir, LOW); 
        delay(1000);

        // Motor no gira
        digitalWrite (engine->pin_enable, LOW); 
        delay(500);

        // Motor gira en sentido inverso
#if DEBUG_ENGINES
        print_debug(ES, stdout, CCYAN, COLOR_NORMAL, "    %s - Turn side 2", engine->name);
#endif
        digitalWrite (engine->pin_dir, HIGH);
        delay(1000);

        // Motor no gira
        digitalWrite (engine->pin_dir, LOW); 
*/



#if DEBUG_ENGINES
    print_debug(ES, stdout, CGREEN, COLOR_NORMAL, "    ENGINE MOVE - Done");
#endif
}

// === SETUP === ==========================================================

void engines_setup(long int now) {
#if DEBUG_ENGINES
    print_debug(ES, stdout, CPURPLE, COLOR_NORMAL, "INI");
#endif

        engine_setup("Front left 1", ENGINE_FRONT_LEFT, ENGINE_FRONT_LEFT_CW, ENGINE_FRONT_LEFT_DIR, ENGINE_FRONT_LEFT_ENABLE);
        engine_setup("Lateral left 2", ENGINE_LATERAL_LEFT, ENGINE_LATERAL_LEFT_CW, ENGINE_LATERAL_LEFT_DIR, ENGINE_LATERAL_LEFT_ENABLE);
        engine_setup("Back left 3", ENGINE_BACK_LEFT, ENGINE_BACK_LEFT_CW, ENGINE_BACK_LEFT_DIR, ENGINE_BACK_LEFT_ENABLE);
        engine_setup("Back right 4", ENGINE_BACK_RIGHT, ENGINE_BACK_RIGHT_CW, ENGINE_BACK_RIGHT_DIR, ENGINE_BACK_RIGHT_ENABLE);
        engine_setup("Lateral right 5", ENGINE_LATERAL_RIGHT, ENGINE_LATERAL_RIGHT_CW, ENGINE_LATERAL_RIGHT_DIR, ENGINE_LATERAL_RIGHT_ENABLE);
        engine_setup("Front right 6", ENGINE_FRONT_RIGHT, ENGINE_FRONT_RIGHT_CW, ENGINE_FRONT_RIGHT_DIR, ENGINE_FRONT_RIGHT_ENABLE);

#if ENGINE_TEST
    // Full test
    while (1) {
#else
    // Decide whether to use test or not (Quick Start)
    if (!QUICK_START) {
#endif
#if OPTIMIZE
        Serial.println(F("ENGINES Test started"));
#else
        print_debug("ENGINE-SETUP", stderr, CYELLOW, 0, "Testing engines!");
#endif
        engine_test(ENGINE_FRONT_LEFT);
        engine_test(ENGINE_LATERAL_LEFT);
        engine_test(ENGINE_BACK_LEFT);
        engine_test(ENGINE_BACK_RIGHT);
        engine_test(ENGINE_LATERAL_RIGHT);
        engine_test(ENGINE_FRONT_RIGHT);
#if OPTIMIZE
        Serial.println(F("ENGINES Test finished"));
#else
        print_debug("ENGINE-SETUP", stderr, CYELLOW, 0, "Test finished");
#endif
#if ENGINE_TEST
    }
#else
    }
#endif

#if DEBUG_ENGINES
    print_debug(ES, stdout, CPURPLE, COLOR_NORMAL, "DONE");
#endif

    /*
    // Engine tests
    while (0) {
        Serial.println("Sleeping...");
        delay(600000);
    }
    if (0) {
        EngineConfig *engine=&(engines_config.engines[ENGINE_LATERAL_RIGHT]);
        while (1) {
            Serial.println("STOP");
            engine_set(engine, ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
            delay(1000);
            Serial.println("BLUE");
            engine_set(engine, ENGINE_BLUE, ENGINE_GEAR_FULL);
            delay(2000);
            Serial.println("STOP");
            engine_set(engine, ENGINE_NOCOLOR, ENGINE_GEAR_STOP);
            delay(1000);
            Serial.println("RED");
            engine_set(engine, ENGINE_RED, ENGINE_GEAR_FULL);
            delay(2000);
        }
    }
    while (0) {
        Serial.println("STOP");
        engine_move("", ROVER_MOVE_STOP);
        delay(2000);
        Serial.println("A");
        engine_move("", ROVER_ROLL_LEFT);
        delay(5000);
        Serial.println("STOP");
        engine_move("", ROVER_MOVE_STOP);
        delay(2000);
        Serial.println("B");
        engine_move("", ROVER_ROLL_RIGHT);
        delay(5000);
        Serial.println("STOP");
        engine_move("", ROVER_MOVE_STOP);
        delay(30001);
    }
    */

}



// === LOOP === ===========================================================

void engines_loop(long int now) {
#if MODULE_ENGINES
#if DEBUG_ENGINES
    // print_debug("ENGINES-LOOP", stdout, CYELLOW, COLOR_NORMAL, "Engine");
#endif
    // Pad 1 (Z: -1:down +1:up   R: -1:left +1:right)
    if (rov.userrequest.z>0) {
        engine_move("ENGINES", ROVER_MOVE_FORWARD);
    } else if (rov.userrequest.z<0) {
        engine_move("ENGINES", ROVER_MOVE_BACKWARD);
    } else if (rov.userrequest.r<0) {
        engine_move("ENGINES", ROVER_LOOK_LEFT);
    } else if (rov.userrequest.r>0) {
        engine_move("ENGINES", ROVER_LOOK_RIGHT);

    // Pad 2 (X: -1:down +1:up   Y: -1:left +1:right)
    } else if (rov.userrequest.x>0) {
        engine_move("ENGINES", ROVER_MOVE_UP);
    } else if (rov.userrequest.x<0) {
        engine_move("ENGINES", ROVER_MOVE_DOWN);
    } else if (rov.userrequest.y<0) {
        engine_move("ENGINES", ROVER_MOVE_LEFT);
    } else if (rov.userrequest.y>0) {
        engine_move("ENGINES", ROVER_MOVE_RIGHT);
    } else {
        // If no movement, stop
        engine_move("ENGINES", ROVER_MOVE_STOP);
    }
#endif
}
