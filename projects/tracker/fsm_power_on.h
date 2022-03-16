#if !defined(FSMpower_H)
#define FSMpower_H

#include "configs.h"
#include "settings.h"
#include "sim7000.h"
#include "Adafruit_FONA.h"

extern void rtc_sleep(uint64_t delay);
extern Adafruit_FONA_LTE fona;
extern Sim7000 sim_device;
extern Settings set_handler;
extern states state;
extern bool pending;
extern Sim7000 sim_device;
extern time_t lastInterval;
extern int gpsErrors,
    gsmErrors;

class FSMpower
{

public:
    FSMpower();

    void run(),
        setup(Sim7000 *sim_device);

private:
    uint64_t stateChange;
    Sim7000 *_sim_device;
    int tries;
};

#endif // FSMpower_H