/*
    Testing deep sleep and watchdog

    - Watchdog will reset the microprocessor 3 time
      during this time the led will be flashing fast
    - Then the led will flash slow and the microprocessor
      will sleep for 5 seconds
    - While sleeping, the microprocessor can be waken up
      by setting high the pin 36
    - The seq number will increase with every reset and
      it will be overflown in 255 or set to 0 with rst button

    TODO light sleep not working
*/

#define DEBUG true

#define S_TO_uS_FACTOR 1000000ULL /* Conversion factor for seconds to micro seconds */

#define SIM_PWR 5 // NRESET
#define PWRKEY 2  // test led

#include "energy.h"
#include <esp_task_wdt.h>
#include "esp_system.h"

unsigned long last_blink;
int val;

#define wdtTimeout 3000000L // time in us to trigger the watchdog

void setup()
{

    if (DEBUG)
    {
        // Initialize serial port
        Serial.begin(115200); //  921600 in the serial monitor scaled x8
        Serial.println();
        Serial.println();
        Serial.println();
    }

    rtc_handle_wakeup();

    reduceFreq();

    watchdogConfig(wdtTimeout);

    // Blink led
    pinMode(PWRKEY, OUTPUT);
    val = LOW;
    digitalWrite(PWRKEY, val);
}

void loop()
{
    Serial.print("Seq: ");
    Serial.println(seq_num++);
    if (wdg_rst_count > 2)
    {
        for (int i = 0; i < 6; i++)
        {
            val = !val;
            digitalWrite(PWRKEY, val);
            delay(1000);
            timerWrite(timer, 0); // reset timer (feed watchdog)
        }

        Serial.println("Power off led before sleeping");
        digitalWrite(PWRKEY, LOW);

        rtc_deep_sleep(5);
    }
    while (1)
    {
        if (millis() - last_blink > 300)
        {
            last_blink = millis();
            val = !val;
            digitalWrite(PWRKEY, val);
        }
    }
}
