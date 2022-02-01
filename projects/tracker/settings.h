#if !defined(SET_H)
#define SET_H

#include <EEPROM.h>
#include "configs.h"
#include "Adafruit_FONA.h"

extern int //Tcheck, // Time interval for power check (0 - 255) min (deprecated)
    MAX_ERRORS,    // (0 - 255)
    Tint,          // Time interval for position updates (0 - 65535) min
    TintB,         // Time interval for position updates on battery (0 - 65535) min
    TGPS,          // Time allow for fixing location (0 - 255) min
    TGPSB,         // Time allow for fixing location on battery (0 - 255) min
    SMART,         // Smart behaviour on battery (0 - 1)
    TsendB,        // Time allow for sending data on battery (0 - 255) min
    Tsend;         // Time allow for sending data (0 - 255) min

class Settings
{

public:
    Settings();

    void run(),
        setup(),
        setTcheck(int val),
        setMAX_ERRORS(int val),
        setTint(unsigned int val),
        setTintB(unsigned int val),
        setTGPS(int val),
        setTGPSB(int val),
        setSMART(int val),
        setTsendB(int val),
        setTsend(int val);
};

#endif // SET_H