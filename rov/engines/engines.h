#ifndef ENGINES_H
#define ENGINES_H

// Engines
#define ENGINE_FRONT_RIGHT 0
#define ENGINE_LATERAL_RIGHT 1
#define ENGINE_BACK_RIGHT 2
#define ENGINE_BACK_LEFT 3
#define ENGINE_LATERAL_LEFT 4
#define ENGINE_FRONT_LEFT 5

// Engine direction
#define ENGINE_GEAR_STOP 0          // Stop
#define ENGINE_GEAR_HALF 1          // Half speed
#define ENGINE_GEAR_FULL 2          // Full speed
#define ENGINE_NOCOLOR 0            // No color - No direction
#define ENGINE_BLUE 1               // Blue direction
#define ENGINE_RED  2               // Red direction


// Rover movements
#define ROVER_MOVE_STOP 0
#define ROVER_MOVE_UP 1             // Translate up
#define ROVER_MOVE_DOWN 2           // Translate down
#define ROVER_MOVE_FORWARD 3        // Translate forward
#define ROVER_MOVE_BACKWARD 4       // Translate backward
#define ROVER_MOVE_LEFT 5           // Translate left
#define ROVER_MOVE_RIGHT 6          // Translate right
#define ROVER_LOOK_LEFT 7           // Look left - Yaw left
#define ROVER_LOOK_RIGHT 8          // Look right - Yaw right
#define ROVER_LOOK_UP 9             // Look up - Pitch up
#define ROVER_LOOK_DOWN 10          // Look down - Pitch down
#define ROVER_ROLL_LEFT 11          // Roll left
#define ROVER_ROLL_RIGHT 12         // Roll right

#endif
