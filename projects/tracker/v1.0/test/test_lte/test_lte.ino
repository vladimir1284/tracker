
#include <sim7000.h>

enum states
{
    // Power connected
    MODEM_PWR_ON,
    LTE_NETWORK,
    CELL_ID,
    CONNECT,
    UPLOAD
};

states state;
String imei, cellID;
Sim7000 sim;

void setup()
{
    Serial.begin(115200);
    sim.setup();
    state = MODEM_PWR_ON;
    sim.powerON();
}

void loop()
{
    String msg;

    switch (state)
    {
    case MODEM_PWR_ON:
        imei = sim.init();
        if (imei.equals(String(NULL)))
        {
            if (DEBUG)
            {
                Serial.println("Delaying 10s and retrying...");
            }
            delay(10000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("IMEI: " + imei);
                Serial.println("Delaying 3s...");
            }
            state = LTE_NETWORK;
            delay(3000);
        }
        break;

    case LTE_NETWORK:
        if (sim.connect2LTE())
        {
            state = CELL_ID;
        }
        else
        {
            delay(1000);
        }
        break;

    case CELL_ID:
        cellID = sim.getCellID();
        if (cellID.equals(String(NULL)))
        {
            delay(1000);
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("UE system information: " + cellID);
            }
            state = CONNECT;
        }
        break;

    case CONNECT:
        if (sim.connect2Internet())
        {
            state = UPLOAD;
        }
        else
        {
            if (DEBUG)
            {
                Serial.println("Delaying 10s and retrying...");
            }
            delay(10000);
        }
        break;

    case UPLOAD:
        msg = imei + ",0,1,4106,0,0,LTE," + cellID;
        if (DEBUG)
        {
            Serial.print("Uploading datagram: ");
            Serial.println(msg);
        }
        sim.uploadData(msg);
        // Terminate test
        if (DEBUG)
        {
            Serial.println("Stop the execution!");
        }
        while (1)
            ;
        break;

    default:
        break;
    }
}
