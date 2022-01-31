#if !defined(FSMpower_H)
#define FSMpower_H

#include "configs.h"
#include "settings.h"
#include "sim7000.h"
#include "Adafruit_FONA.h"

extern void rtc_sleep(unsigned long delay);
extern Adafruit_FONA_LTE fona;
extern Sim7000 sim_device;

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
    unsigned long stateChange;
    Sim7000 *_sim_device;
};

#endif // FSMpower_H