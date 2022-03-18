#if !defined(RTC_H)
#define RTC_H

#include <Arduino.h>

// ---------------------------------------------
// RTC memory to hold state values on deep sleep
RTC_DATA_ATTR modes mode;
RTC_DATA_ATTR states state;
RTC_DATA_ATTR time_t lastInterval;

RTC_DATA_ATTR int gpsErrors,
    gsmErrors;
RTC_DATA_ATTR int Tcheck, // Time interval for power check (0 - 255) min
    MAX_ERRORS,           // (0 - 255)
    Tint,                 // Time interval for position updates (0 - 65535) min
    TintB,                // Time interval for position updates on battery (0 - 65535) min
    TGPS,                 // Time allow for fixing location (0 - 255) min
    TGPSB,                // Time allow for fixing location on battery (0 - 255) min
    SMART,                // Smart behaviour on battery (0 - 1)
    TsendB,               // Time allow for sending data on battery (0 - 255) min
    Tsend;                // Time allow for sending data (0 - 255) min

RTC_DATA_ATTR char imei[16]; // MUST use a 16 character buffer for IMEI!
RTC_DATA_ATTR int imei_len;
RTC_DATA_ATTR byte seq_num;
RTC_DATA_ATTR bool pending;
RTC_DATA_ATTR char msg[MSG_SIZE];

// ---------------------------------------------
// Low energy compsumption delay (in seconds)
void rtc_light_sleep(uint64_t delay)
{
    // Hold on IO pins value
    gpio_hold_en((gpio_num_t)SIM_PWR);
    gpio_hold_en((gpio_num_t)PWRKEY);
    gpio_deep_sleep_hold_en();

    esp_sleep_enable_timer_wakeup(delay * S_TO_uS_FACTOR);
    esp_light_sleep_start();

    // Unlock IO pins value
    gpio_hold_dis((gpio_num_t)SIM_PWR);
    gpio_hold_dis((gpio_num_t)PWRKEY);
}

void rtc_handle_wakeup()
{
    // Unlock IO pins value
    gpio_hold_dis((gpio_num_t)SIM_PWR);
    gpio_hold_dis((gpio_num_t)PWRKEY);

    // We only keep RTC values when waking up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER)
    {
        state = READ_GPS;
        lastInterval = 0;
        gpsErrors = 0;
        gsmErrors = 0;
        pending = false;
        imei_len = 0;
        seq_num = 0;
    }
}

void rtc_sleep(uint64_t delay)
{
    esp_sleep_enable_timer_wakeup(delay);

    // Hold on IO pins value
    gpio_hold_en((gpio_num_t)SIM_PWR);
    gpio_hold_en((gpio_num_t)PWRKEY);
    gpio_deep_sleep_hold_en();

    // Wakeup on energy connection
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

    // Going to sleep
    esp_deep_sleep_start();
}

// ---------------------------------------------
// Detect battery powered debouncing input signal
void detectMode()
{
    modes initial_mode = mode;
    if (mode == POWER_ON)
    {
        for (int i = 0; i < 3; i++)
        {
            if (digitalRead(PIN12V))
            {
                mode = POWER_ON;
                break;
            }
            mode = BATTERY;
            delay(5);
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            if (!digitalRead(PIN12V))
            {
                mode = BATTERY;
                break;
            }
            mode = POWER_ON;
            delay(5);
        }
    }
    if (initial_mode != mode)
    {
        state = READ_GPS; // Force update on mode change
        if (DEBUG)
        {
            Serial.print("Mode: ");
            Serial.println(mode);
        }
        
    }
}

#endif // RTC_H