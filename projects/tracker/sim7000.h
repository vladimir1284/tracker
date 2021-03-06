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
        uploadData(),
        checkSMS(),
        netStatus(),
        turnON(),
        configure();
    void setup(),
        powerOFF(),
        powerON(),
        hardReset(),
        reset();

    char smsBuffer[MAX_INPUT_LENGTH];

};

#endif // SIM7000_H