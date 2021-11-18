#if !defined(FSM_H)
#define FSM_H

#include "configs.h"
#include "gps.h"

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

class FSM
{

public:
    FSM();

    void run(),
        setup();

private:
    int tGPS; // Time allow for fixing location

    // void trunOnSpeed0(),
    //     trunOffSpeed0();
};

#endif // FSM_H