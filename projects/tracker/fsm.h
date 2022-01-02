#if !defined(FSM_H)
#define FSM_H

#include <EEPROM.h>
#include "configs.h"
#include "gps.h"
#include "sim.h"

enum states
{
    // Power connected
    IDLE,
    ENERGY,
    READ_GPS,
    SEND_DATA,
    UPDATE_CFG,
    ERROR,
    // Battery powered
    SLEEP,
    BATTERY,
    BAT_GPS,
    BAT_SEND,
    BAT_CFG,
    BAT_ERROR
};

class SIM;

class FSM
{

public:
    FSM();

    void run(),
        setup(int pin, GPS *gps, SIM *sim),
        setTcheck(int val),
        setMAX_ERRORS(int val),
        setTint(unsigned int val),
        setTintB(unsigned int val),
        setTGPS(int val),
        setTGPSB(int val),
        setSMART(int val),
        setTsendB(int val),
        setTsend(int val);

private:
    int pin12V,
        gpsErrors,
        gsmErrors;

    unsigned long lastInterval,
        lastCheck,
        stateChange;

    unsigned int Tcheck, // Time interval for power check (0 - 255) min
        MAX_ERRORS,      // (0 - 255)
        Tint,            // Time interval for position updates (0 - 65535) min
        TintB,           // Time interval for position updates on battery (0 - 65535) min
        TGPS,            // Time allow for fixing location (0 - 255) min
        TGPSB,           // Time allow for fixing location on battery (0 - 255) min
        SMART,           // Smart behaviour on battery (0 - 1)
        TsendB,          // Time allow for sending data on battery (0 - 255) min
        Tsend;           // Time allow for sending data (0 - 255) min

    states state;

    GPS *_gps;
    SIM *_sim;
};

#endif // FSM_H