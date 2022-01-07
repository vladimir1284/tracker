#if !defined(RTC_H)
#define RTC_H

// ---------------------------------------------
// RTC memory to hold state values on deep sleep
RTC_DATA_ATTR states state;
RTC_DATA_ATTR unsigned long lastInterval,
    millisOffset;
RTC_DATA_ATTR int gpsErrors,
    gsmErrors;
RTC_DATA_ATTR bool simOn,
    gpsOn;
RTC_DATA_ATTR GPSdataStruct gpsData;
RTC_DATA_ATTR int Tcheck, // Time interval for power check (0 - 255) min
    MAX_ERRORS,           // (0 - 255)
    Tint,                 // Time interval for position updates (0 - 65535) min
    TintB,                // Time interval for position updates on battery (0 - 65535) min
    TGPS,                 // Time allow for fixing location (0 - 255) min
    TGPSB,                // Time allow for fixing location on battery (0 - 255) min
    SMART,                // Smart behaviour on battery (0 - 1)
    TsendB,               // Time allow for sending data on battery (0 - 255) min
    Tsend;                // Time allow for sending data (0 - 255) min
// ---------------------------------------------

unsigned long getMillis()
{
    return millisOffset + millis();
}

void rtc_handle_wakeup()
{
    // We only keep RTC values when waking up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER)
    {
        state = ENERGY;
        lastInterval = 0;
        millisOffset = 0;
        gpsErrors = 0;
        gsmErrors = 0;
        gpsData.pending = false;
        Tcheck = 0;
    }
}

void rtc_sleep(unsigned long delay)
{
    esp_sleep_enable_timer_wakeup(delay);

    // This will be the offset on wake up
    millisOffset = getMillis() + delay / 1000;

    // Hold on IO pins value
    gpio_hold_en((gpio_num_t)SIM_PWR);
    gpio_hold_en((gpio_num_t)GPS_PWR);
    gpio_deep_sleep_hold_en();

    // Going to sleep
    esp_deep_sleep_start();
}

#endif // RTC_H