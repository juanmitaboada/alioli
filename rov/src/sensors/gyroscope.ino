#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "monitor/monitor.h"
#include "gyroscope.h"

GyroscopeConfig gyroscope_config;
const char *GS="GS";
const char *GL="GL";

// === SETUP === ==========================================================
void gyroscope_setup(long int now) {
    unsigned short int counter=0, found=0;
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("GS: INI: "));
#else
    print_debug(GS, stdout, CPURPLE, 0, "INI");
#endif
#endif

#ifdef GYROSCOPE_MPU
    // Initialize MPU6050
    counter=0;
    found=0;
    found = mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G, GYROSCOPE_MPU);
    while (!found) {
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("GS: Couldn't find a valid MPU6050 sensors, check wiring or I2C ADDR!"));
#else
        print_debug(GS, stdout, CPURPLE, 0, "Could not find a valid MPU6050 sensor, check wiring or I2C ADDR!");
#endif
        delay(500);
#endif

        counter++;
        if (counter<10) {
            found = mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G, GYROSCOPE_MPU);
        } else {
            break;
        }
    }
    gyroscope_config.mpu_enabled = found;
    if (!found) {
#if GYROSCOPE_MPU_MUST_EXISTS
        // Close in a loop just in case (but lights_error is already a closed bucle)
        while (1) {
#if OPTIMIZE
            lights_error(NULL, __FILE__, __LINE__);
#else
            lights_error("Could not find a valid MPU6050 sensor, we won't continue!", __FILE__, __LINE__);
#endif
        }
#else
        print_debug(GS, stdout, CPURPLE, 0, "Could not find a valid MPU6050 sensor, we will keep going with NO MPU!");
#endif
    }
#else
    gyroscope_config.mpu_enabled = 0;
#endif

    // Initialize BNO055
    counter=0;
    found=0;
    // bno = Adafruit_BNO055(55); <------------------------------- FROM EXAMPLE
    bno = Adafruit_BNO055(GYROSCOPE_BNO);
    found = bno.begin();
    while (!found) {
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("GS: Couldn't find a valid BNO055 sensors, check wiring or I2C ADDR!"));
#else
        print_debug(GS, stdout, CPURPLE, 0, "Could not find a valid BNO055 sensor, check wiring or I2C ADDR!");
#endif
        delay(500);
#endif
        counter++;
        if (counter<10) {
            found = bno.begin();
        } else {
            break;
        }
    }
    if (!found) {
        // Close in a loop just in case (but lights_error is already a closed bucle)
        // while (1) {
        //     lights_error("Could not find a valid BNO055 sensor, we won't continue!", __FILE__, __LINE__);
        // }
    }

#ifdef GYROSCOPE_MPU
    // If MPU enabled
    if (gyroscope_config.mpu_enabled) {
        // Calibrate gyroscope. The calibration must be at rest.
        // If you don't want calibrate, comment this line.
        mpu.calibrateGyro();

        // Set threshold sensivty. Default 3.
        // If you don't want use threshold, comment this line or set 0.
        mpu.setThreshold(3);
    }
#endif

    // BNO - Enable external clock for better accuracy
    bno.setExtCrystalUse(true);

    // Set local config
    gyroscope_config.nextevent = 0;
    gyroscope_config.lasttime = now;

    // Set environment
    rov.environment.temperaturegy = 0.0;
    rov.environment.acelerometer.Tmp = 0.0;
    rov.environment.acelerometer.roll = 0.0;
    rov.environment.acelerometer.pitch = 0.0;
    rov.environment.acelerometer.yaw = 0.0;
    rov.environment_newdata = 1;
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("GS: DONE"));
#else
    print_debug(GS, stdout, CPURPLE, 0, "DONE");
#endif
#endif
}

// === LOOP === ===========================================================

float pitch = 0;
float roll = 0;
float yaw = 0;
void gyroscope_loop(long int now) {
#ifdef GYROSCOPE_MPU
    float timeStep = 0.01;
    Vector norm;
#endif
    sensors_event_t event; 
    int8_t temp = 0;

    // Check gyroscope lookup
    if (gyroscope_config.nextevent<now) {

        // Set next event
        gyroscope_config.nextevent = now+GYROSCOPE_LOOKUP_MS;

#ifdef GYROSCOPE_MPU
        // If MPU enabled
        if (gyroscope_config.mpu_enabled) {

            // === MPU ===
            norm = mpu.readNormalizeGyro();

            // Calculate Pith, Roll and Yaw
            pitch = pitch + norm.YAxis * timeStep;
            roll = roll + norm.XAxis * timeStep;
            yaw = yaw + norm.ZAxis * timeStep;

#if DEBUG_SENSORS_GYROSCOPE
            // Output raw
            Serial.print(F(" Pitch = "));
            Serial.print(pitch);
            Serial.print(F(" Roll = "));
            Serial.print(roll);  
            Serial.print(F(" Yaw = "));
            Serial.println(yaw);
#endif
        }
#endif

        // Get BNO information
        bno.getEvent(&event);
        temp = bno.getTemp();

#if DEBUG_SENSORS_GYROSCOPE

        Serial.print(F("Roll: "));
        Serial.print(event.orientation.roll, 4);
        Serial.print(F("\tPitch: "));
        Serial.print(event.orientation.pitch, 4);
        Serial.print(F("\tHeadding: "));
        Serial.print(event.orientation.heading, 4);
        Serial.print(F("\r\n"));

        /*
        // Display the floating point data
        Serial.print(F("X: "));
        Serial.print(event.orientation.x, 4);
        Serial.print(F("\tY: "));
        Serial.print(event.orientation.y, 4);
        Serial.print(F("\tZ: "));
        Serial.print(event.orientation.z, 4);

        Serial.print(F("Acc X (ROLL): "));
        Serial.print(event.acceleration.x, 4);
        Serial.print(F("\tAcc Y (PITCH): "));
        Serial.print(event.acceleration.y, 4);
        Serial.print(F("\tAcc Z (HEADING): "));
        Serial.print(event.acceleration.z, 4);

        imu::Vector<3> li_ac = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        Serial.print(F("Li Acc X: "));
        Serial.print(li_ac.x(), 4);
        Serial.print(F("\tLi Acc Y: "));
        Serial.print(li_ac.y(), 4);
        Serial.print(F("\tLi Acc Z: "));
        Serial.print(li_ac.z(), 4);
        */

        Serial.print(F("\tCurrent Temperature: "));
        Serial.print(temp);
        Serial.println(F(" C"));
#endif

        // Save data in the main structure
        rov.environment.temperaturegy = temp;
        rov.environment.acelerometer.Tmp = temp;
        rov.environment.acelerometer.roll = event.orientation.roll;
        rov.environment.acelerometer.pitch = event.orientation.pitch;
        rov.environment.acelerometer.yaw = event.orientation.heading;
        rov.environment_newdata = 1;

#if DEBUG_SENSORS_GYROSCOPE
        print_debug(GL, stdout, CYELLOW, 0, "Roll=%d Pitch:%d Yaw:%d Temp:%d", (int)rov.environment.acelerometer.roll, (int)rov.environment.acelerometer.pitch, (int)rov.environment.acelerometer.yaw, rov.environment.acelerometer.Tmp);
#endif

    }
}





/*
void gyroscope_loop_old(long int now) {
    int16_t AcX=0,AcY=0,AcZ=0,Tmp=0,GyX=0,GyY=0,GyZ=0;
    float temperature=0.0, diff_time=0.0;
    float accx=0.0,accy=0.0, gyx=0.0, gyy=0.0, gyz=0.0;
#if DEBUG_SENSORS_GYROSCOPE
    char senstemp[20]="";
#endif

    // Check gyroscope lookup
    if (gyroscope_config.nextevent<now) {

        // Set next event
        gyroscope_config.nextevent = now+GYROSCOPE_LOOKUP_MS;

        // === GYROSCOPE ===
        Wire.beginTransmission(GYROSCOPE_SENSOR_1);

        Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)

        if (!Wire.endTransmission(false)) {
            if (Wire.requestFrom(GYROSCOPE_SENSOR_1, 14, true)==14) {  // request a total of 14 registers
                if (Wire.available()>0) {
                    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
                }
                if (Wire.available()>0) {
                    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
                }
                if (Wire.available()>0) {
                    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
                }
                if (Wire.available()>0) {
                    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
                }
                if (Wire.available()>0) {
                    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
                }
                if (Wire.available()>0) {
                    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
                }
                if (Wire.available()>0) {
                    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
                }
                Wire.endTransmission(true);

                // Equation for temperature in degrees C from datasheet
                temperature = Tmp / 340.00 + 36.53;

                // Calculate anglefrom acelerometer
                accy = atan(-1*(AcX/GYROSCOPE_A_R)/sqrt(pow((AcY/GYROSCOPE_A_R),2) + pow((AcZ/GYROSCOPE_A_R),2)))*GYROSCOPE_RAD_TO_DEG;
                accx = atan((AcY/GYROSCOPE_A_R)/sqrt(pow((AcX/GYROSCOPE_A_R),2) + pow((AcZ/GYROSCOPE_A_R),2)))*GYROSCOPE_RAD_TO_DEG;

                // Calculate angle from gyroscope
                gyx = GyX/GYROSCOPE_G_R;
                gyy = GyY/GYROSCOPE_G_R;
                gyz = GyZ/GYROSCOPE_G_R;

                // Calculate time passed and update lasttime
                diff_time = millis() - gyroscope_config.lasttime;
                gyroscope_config.lasttime += diff_time;

                // Calculate angles
                rov.pos.angx = 0.98 *(rov.pos.angx+gyx*diff_time) + 0.02*accx;
                rov.pos.angy = 0.98 *(rov.pos.angy+gyy*diff_time) + 0.02*accy;

                // Calculate YAW respecto the time
                rov.pos.angz = (rov.pos.angz+gyz*diff_time);

                // Save status
                // rov.pos.AcX = AcX;
                // rov.pos.AcY = AcY;
                // rov.pos.AcZ = AcZ;
                rov.environment.temperaturegy = temperature;
                // rov.pos.GyX = GyX;
                // rov.pos.GyY = GyY;
                // rov.pos.GyZ = GyZ;

#if DEBUG_SENSORS_GYROSCOPE
                print_debug(GL, stdout, CYELLOW, 0, "Acx=%5d AcY=%5d AcZ=%5d Tmp=%s GyX=%4d GyY=%4d GyZ=%4d AngX=%s AngY=%s AngZ=%s", AcX, AcY, AcZ, dtostrf(temperature, 8, 4, senstemp), GyX, GyY, GyZ, dtostrf(rov.pos.angx, 8, 4, senstemp), dtostrf(rov.pos.angy, 8, 4, senstemp), dtostrf(rov.pos.angz, 8, 4, senstemp));
#endif
            } else {
                print_debug(GL, stdout, "red", 0, "%s:%d: Error reading BUS!", __FILE__, __LINE__);
                Wire.endTransmission(true);
            }
        } else {
            print_debug(GL, stdout, "red", 0, "%s:%d: Error reading BUS!", __FILE__, __LINE__);
        }
    }
}
*/

