#if !defined(ENERGY_H)
#define ENERGY_H

#include <Arduino.h>
#include "config.h"
#include "sim7000.h"

Sim7000 sim;

// --------------- Pinout ------------------

#define STATUSLED 12 // blue led near ESP32
#define BAT_ADC 35
#define PIN12V 36 // Input pin for checking 12V connection

// --------------------------------------

// --------------- Constants ------------------

#define ONE_DAY_uS 1440000000ULL     // 24 Minutes for testing
#define MIN_VBAT 2900                // Minimum operational battery voltage (mV)
#define VBAT_SAMPLES 64              // Number of oversampling repetitions for reading battery voltage
#define VBAT_COEF 0.027              // Coefficient converting the sum of VBAT_SAMPLES into battery voltage (mV)
#define PWR_PIN_BITMASK 0x1000000000 // 2^PIN12V 0b10000,00000000,00000000,00000000,00000000

// --------------------------------------

// --------------- watchdog ------------------

#define WDT_TIMEOUT 60000000ULL // time in us to trigger the watchdog

hw_timer_t *timer = NULL;

// ------- watchdog reset --------------
void IRAM_ATTR resetModule()
{
    ets_printf("reboot\n");
    esp_restart();
}
//  -------------------------------------

// --------------------------------------

// --------- Variables ---------------
bool charging; // Weather the charger is connected
uint16_t vbat; // Battery voltage
esp_sleep_wakeup_cause_t wakeup_reason;
esp_reset_reason_t reset_reason;

// --------------------------------------

// ---------------------------------------------
// RTC memory to hold state values on deep sleep
RTC_NOINIT_ATTR time_t lastInterval, lastWakeup;
RTC_NOINIT_ATTR int wdg_rst_count;
RTC_NOINIT_ATTR byte seq_num;

// -----------------------------------------

// ------- watchdog begin --------------
void watchdogConfig(uint64_t wdtTimeout)
{
    timer = timerBegin(0, 10, true);                 // timer 0, div 10
    timerAttachInterrupt(timer, &resetModule, true); // attach callback
    timerAlarmWrite(timer, wdtTimeout, false);       // set time in us
    timerAlarmEnable(timer);
}
//  -------------------------------------

// ------- Slowdown for energy saving ------
void reduceFreq()
{
    setCpuFrequencyMhz(10);
    if (DEBUG)
    {
        uint32_t Freq;
        Freq = getCpuFrequencyMhz();
        Serial.print("CPU Freq = ");
        Serial.print(Freq);
        Serial.println(" MHz");
        Freq = getXtalFrequencyMhz();
        Serial.print("XTAL Freq = ");
        Serial.print(Freq);
        Serial.println(" MHz");
        Freq = getApbFrequency();
        Serial.print("APB Freq = ");
        Serial.print(Freq);
        Serial.println(" Hz");
    }
}
// -------------------------------------

// ---------------------------------------------
// Lock IO pins before sleeping
void lockIOpins()
{
    gpio_hold_en((gpio_num_t)SIM_PWR);
    gpio_hold_en((gpio_num_t)PWRKEY);
    gpio_hold_en((gpio_num_t)STATUSLED);
    gpio_deep_sleep_hold_en();
}

// ---------------------------------------------
// Unlock IO pins after wakeup
void unlockIOpins()
{
    gpio_hold_dis((gpio_num_t)SIM_PWR);
    gpio_hold_dis((gpio_num_t)PWRKEY);
}

// ---------------------------------------------
// Low energy consumption delay (in seconds)
void rtc_light_sleep(int delay)
{
    lockIOpins();

    esp_sleep_enable_timer_wakeup(delay * S_TO_uS_FACTOR);
    esp_light_sleep_start();

    unlockIOpins();
}

// ---------------------------------------------
// Deep sleep (in seconds)
void rtc_deep_sleep(int delay)
{
    esp_sleep_enable_timer_wakeup(delay * S_TO_uS_FACTOR);

    if (DEBUG)
    {
        Serial.print("Sleeping for: ");
        Serial.print(delay);
        Serial.println("s...");
    }

    lockIOpins();

    uint64_t BUTTON_PIN_BITMASK = 0;
    if (!charging)
    {
        // Wakeup on energy connection
        BUTTON_PIN_BITMASK = PWR_PIN_BITMASK;

        if (DEBUG)
        {
            Serial.print("BUTTON_PIN_BITMASK = 0x");
            Serial.print(String((unsigned int)(BUTTON_PIN_BITMASK >> 32), 16));
            Serial.println("00000000");
        }
    }

    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);

    // Disable watchdog
    timerDetachInterrupt(timer);

    // Going to sleep
    esp_deep_sleep_start();
}

// ---------------------------------------------
// Handle wakeup process
void rtc_handle_wakeup()
{
    // Find out the reason for waking up
    wakeup_reason = esp_sleep_get_wakeup_cause();
    if (DEBUG)
    {
        switch (wakeup_reason)
        {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by external signal using RTC_IO");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup caused by ULP program");
            break;
        default:
            Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
            break;
        }
    }

    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
    {
        if (DEBUG)
        {
            uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
            int pin = (unsigned int)((log(GPIO_reason)) / log(2));
            Serial.print("GPIO that triggered the wake up: GPIO");
            Serial.println(pin);
        }
        // Check for too short interval
        time_t now, slept_time, tracking_interval_s;
        time(&now);
        slept_time = now - lastInterval;
        tracking_interval_s = TRACKING_INTERVAL * MIN_TO_S_FACTOR;
        if (DEBUG)
        {
            Serial.print("Sleep time: ");
            Serial.print(slept_time);
            Serial.println("s");
            Serial.print("Interval: ");
            Serial.print(tracking_interval_s);
            Serial.println("s");
        }
        if (slept_time < tracking_interval_s)
        {
            if (DEBUG)
            {
                Serial.print("Too short interval: ");
                Serial.print(slept_time / 60);
                Serial.println("min!");
            }
            charging = true;
            rtc_deep_sleep(tracking_interval_s - slept_time);
        }
    }

    pinMode(PIN12V, INPUT);

    unlockIOpins();

    reset_reason = esp_reset_reason();
    if (DEBUG)
    {
        Serial.print("Reset reason: ");
        Serial.println(reset_reason);
    }
    if (reset_reason == ESP_RST_SW)
    {
        wdg_rst_count++;
        if (DEBUG)
        {
            Serial.print("Watchdog reset # ");
            Serial.println(wdg_rst_count);
        }
    }
    else
    {
        // We only keep RTC values when waking up from deep sleep or watchdog
        if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED)
        {
            Serial.println("Clear RTC variables");
            lastInterval = 0;
            seq_num = 0;
        }
        time(&lastWakeup); // Mark the wakeup time
        wdg_rst_count = 0; // Reset the watchdog reset count on every wake up
    }
}

// ---------------------------------------------
// Read the battery voltage in mV
uint16_t readBatteryVoltage()
{
    // Read Vbat from analog pin
    unsigned int val = 0;
    for (int i = 0; i < VBAT_SAMPLES; i++)
    {
        val += analogRead(BAT_ADC);
    }
    // This Value will be 0 when connected to USB
    return (uint16_t)(VBAT_COEF * val);
}

// ---------------------------------------------
// Check the battery voltage to prevent extreme discharging
void checkBatteryVoltage(bool turnSIMoff)
{
    vbat = readBatteryVoltage();

    if (vbat > 0 && vbat < MIN_VBAT)
    {
        if (DEBUG)
        {
            Serial.print("Battery voltage: ");
            Serial.print(vbat);
            Serial.print("mV. Under minimum accepted (");
            Serial.print(MIN_VBAT);
            Serial.println("mV).");
        }
        // Power off SIM device
        digitalWrite(PWRKEY, HIGH);
        delay(1500); // Datasheet Toff = 1.2S
        digitalWrite(PWRKEY, LOW);

        if (turnSIMoff)
        {
            sim.powerOFF();
        }
        // sleep for a day
        rtc_deep_sleep(ONE_DAY_uS);
    }
}

// ---------------------------------------------
// Detect battery powered debouncing input signal
void detectCharging()
{
    bool initial_charging = charging;
    if (charging)
    {
        for (int i = 0; i < 3; i++)
        {
            if (digitalRead(PIN12V))
            {
                charging = true;
                break;
            }
            charging = false;
            delay(5);
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            if (!digitalRead(PIN12V))
            {
                charging = false;
                break;
            }
            charging = true;
            delay(5);
        }
    }

    if (DEBUG)
    {
        if (initial_charging != charging)
        {
            if (charging)
            {
                Serial.println("Battery charger connected!");
            }
            else
            {
                Serial.println("Battery charger disconnected!");
            }
        }
    }
}

#endif // ENERGY_H