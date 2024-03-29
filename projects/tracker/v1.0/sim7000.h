#if !defined(SIM_H)
#define SIM_H

#include "config.h"

#include <Arduino.h>
#include <ArduinoHttpClient.h>
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#include <TinyGsmClient.h>

#define MODEM_TX 27
#define MODEM_RX 26

// Your GPRS credentials, if any
const char apn[] = "hologram";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
const char server[] = "trailerrental.pythonanywhere.com";
const char resource[] = "/towit/upload_data";
const int port = 80;

class Sim7000
{

public:
    Sim7000();
    bool connect2LTE(),
        connect2Internet(),
        powerOffGPS(),
        disableGPS(),
        enableGPS(),
        powerOnGPS();
    void setup(),
        powerOFF(),
        powerON(),
        hardReset(),
        reset();
    String init(),
        getCellID(),
        uploadData(String msg),
        getGPS();

private:
    float lat2, lon2, speed2, alt2, accuracy2;
    int vsat2, usat2, year2, month2, day2, hour2, min2, sec2;
};

#endif // SIM_H