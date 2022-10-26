#if !defined(SIM_H)
#define SIM_H

#define DEBUG true

#include <Arduino.h>
#define TINY_GSM_MODEM_SIM7000
#include <TinyGsmClient.h>

#define SIM_PWR 5 // NRESET
#define PWRKEY 4  // GPIO4 -> RTC_GPIO10
#define MODEM_TX 27
#define MODEM_RX 26

// Your GPRS credentials, if any
const char apn[] = "hologram";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
const char server[] = "http://trailerrental.pythonanywhere.com";
const char resource[] = "/towit/upload_data";

class Sim7000
{

public:
    Sim7000();
    bool prepareMessage(),
        uploadData(),
        checkSMS(),
        netStatus(),
        powerOffGPS(),
        powerOnGPS();
    void setup(),
        powerOFF(),
        powerON(),
        hardReset(),
        reset();
    String turnON(),
        getGPS();

private:
    float lat2, lon2, speed2, alt2, accuracy2;
    int vsat2, usat2, year2, month2, day2, hour2, min2, sec2;
};

#endif // SIM_H