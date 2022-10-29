#if !defined(CONTROLLER_H)
#define CONTROLLER_H

#include "time.h"
#include "sim7000.h"
#include "EEPROMsettings.h"

#define UPLOAD_RETRIES 2

enum states
{
    // Power connected
    MODEM_PWR_ON,
    LTE_NETWORK,
    GPS_PWR_ON,
    GPS_FIX,
    GPS_PWR_OFF,
    CELL_ID,
    CONNECT,
    UPLOAD
};

class Controller
{

public:
    Controller();

    bool clear(),
        run(bool charging,
            int vbat,
            int seq,
            int wur,
            int wdgc);

    void setState(states newState),
        setup(Sim7000 *sim_device,
              Settings *settings);

private:
    Sim7000 *sim;
    Settings *stgs;
    states state;
    time_t stateChange,
        now;
    int tries;
    bool timeout;
    String imei,
        cellID,
        gpsData,
        msg,
        config;
};

#endif // CONTROLLER_H