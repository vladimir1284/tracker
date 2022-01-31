#if !defined(SIM7000_H)
#define SIM7000_H

#include "configs.h"
#include <HardwareSerial.h>
#include "Adafruit_FONA.h"

extern modes mode;
extern int imei_len;
extern byte seq_num;
extern bool pending;
extern char imei[16];
extern char msg[MSG_SIZE];
extern HardwareSerial fonaSS;
extern Adafruit_FONA_LTE fona;

class Sim7000
{

public:
    Sim7000();
    bool prepareMessage(),
        uploadData(byte QoS),
        netStatus();
    void setup();

private:
    // int configs_Tcheck,
    //     configs_MAX_ERRORS,
    //     configs_Tint,
    //     configs_TintB,
    //     configs_TsendB,
    //     configs_TGPSB,
    //     configs_SMART,
    //     configs_TGPS,
    //     configs_Tsend;

    // void updateTsend(int value),
    //     updateTint(int value),
    //     updateTGPS(int value),
    //     updateSMART(int value),
    //     updateTGPSB(int value),
    //     updateTsendB(int value),
    //     updateTintB(int value),
    //     updateMAX_ERRORS(int value),
    //     updateTcheck(int value),
    //     updateTrackerID(int value),
    //     getRemoteConfigs();
};

#endif // SIM7000_H