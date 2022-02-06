#if !defined(FSMbattery_H)
#define FSMbattery_H

#include "configs.h"
#include "settings.h"
#include "sim7000.h"
#include "Adafruit_FONA.h"

extern void rtc_sleep(unsigned long delay);
extern void rtc_light_sleep(unsigned long delay);
extern Adafruit_FONA_LTE fona;
extern Sim7000 sim_device;

extern states state;
extern bool pending;
extern Sim7000 sim_device;
extern time_t lastInterval;
extern int gpsErrors,
    gsmErrors;

class FSMbattery
{

public:
    FSMbattery();

    void run(),
        setup(Sim7000 *sim_device);

private:
    unsigned long stateChange;
    Sim7000 *_sim_device;
    int tries;
};

#endif // FSMbattery_H