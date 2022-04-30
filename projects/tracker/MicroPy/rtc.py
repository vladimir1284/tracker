import math
#if not defined(RTC_H)
RTC_H	= #include <Arduino.h>

uint32_t vibrationNumber,
    lastVibrationCheck

# ---------------------------------------------
# RTC memory to hold state values on deep sleep
RTC_DATA_ATTR modes mode
RTC_DATA_ATTR states state
RTC_DATA_ATTR lastInterval
RTC_DATA_ATTR nextWakeUp

RTC_DATA_ATTR gpsErrors,
    gsmErrors
RTC_DATA_ATTR Tcheck, # Time interval for power check (0 - 255) min
    MAX_ERRORS,           # (0 - 255)
    Tint,                 # Time interval for position updates (0 - 65535) min
    TintB,                # Time interval for position updates on battery (0 - 65535) min
    TGPS,                 # Time allow for fixing location (0 - 255) min
    TGPSB,                # Time allow for fixing location on battery (0 - 255) min
    SMART,                # Smart behaviour on battery (0 - 1)
    TsendB,               # Time allow for sending data on battery (0 - 255) min
    Tsend                # Time allow for sending data (0 - 255) min

RTC_DATA_ATTR imei[16] # MUST use a 16 character buffer for IMEI!
RTC_DATA_ATTR imei_len
RTC_DATA_ATTR byte seq_num
RTC_DATA_ATTR pending
RTC_DATA_ATTR msg[MSG_SIZE]
RTC_DATA_ATTR moving

# ---------------------------------------------
# Low energy compsumption delay (in seconds)
def rtc_light_sleep(uint64_t delay):
    # Hold on IO pins value
    gpio_hold_en((gpio_num_t)SIM_PWR)
    gpio_hold_en((gpio_num_t)PWRKEY)
    gpio_deep_sleep_hold_en()

    esp_sleep_enable_timer_wakeup(delay * S_TO_uS_FACTOR)
    esp_light_sleep_start()

    # Unlock IO pins value
    gpio_hold_dis((gpio_num_t)SIM_PWR)
    gpio_hold_dis((gpio_num_t)PWRKEY)

def rtc_sleep(uint64_t delay):
    esp_sleep_enable_timer_wakeup(delay)

    # Compute next wake up
    time(&now)
    nextWakeUp = now + delay / 1e6
    if DEBUG:
        Serial.print("Sleeping for: ")
        Serial.print(delay / 1e6)
        Serial.println("s...")

    # Hold on IO pins value
    gpio_hold_en((gpio_num_t)SIM_PWR)
    gpio_hold_en((gpio_num_t)PWRKEY)
    gpio_deep_sleep_hold_en()

    # Wakeup on energy connection
    uint64_t BUTTON_PIN_BITMASK = PWR_PIN_BITMASK
    if not moving:
        BUTTON_PIN_BITMASK |= VBR_PIN_BITMASK # Wake on vibration

    if DEBUG:
        Serial.print("BUTTON_PIN_BITMASK = ")
        Serial.print((int)(BUTTON_PIN_BITMASK >> 32))


    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH)

    # Going to sleep
    esp_deep_sleep_start()

def rtc_handle_wakeup():
    # Find out the reazon for waking up
    esp_sleep_wakeup_cause_t wakeup_reason
    wakeup_reason = esp_sleep_get_wakeup_cause()
    if DEBUG:
        switch wakeup_reason:
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by external signal using RTC_IO")
            break
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by external signal using RTC_CNTL")
            break
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by timer")
            break
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by touchpad")
            break
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup caused by ULP program")
            break
        default:
            Serial.printf("Wakeup was not caused by deep sleep: %d\n" % (wakeup_reason))
            break

    if ESP_SLEEP_WAKEUP_EXT1:
        uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status()
        pin = (int)((math.log(GPIO_reason)) / math.log(2))
        if DEBUG:
            Serial.print("GPIO that triggered the wake up: GPIO ")
            Serial.println(pin)
        if pin == PINVBR:
            time(&now)
            moving = True
            rtc_sleep(1e6 * (nextWakeUp - now))

    # Unlock IO pins value
    gpio_hold_dis((gpio_num_t)SIM_PWR)
    gpio_hold_dis((gpio_num_t)PWRKEY)

    # We only keep RTC values when waking up from deep sleep
    if wakeup_reason != ESP_SLEEP_WAKEUP_TIMER:
        state = READ_GPS
        lastInterval = 0
        gpsErrors = 0
        gsmErrors = 0
        pending = False
        imei_len = 0
        seq_num = 0
        vibrationNumber = 0
        lastVibrationCheck = 0
        moving = True

# ---------------------------------------------
# Detect movement every MOVDELAY seconds
def detectMovement():
    if not moving:
        time(&now)
        if now - lastVibrationCheck > MOVDELAY:
            lastVibrationCheck = now
            if DEBUG:
                Serial.print("Vibrations detected: ")
                Serial.println(vibrationNumber)
            if vibrationNumber > MOVTHRESHOLD:
                moving = True
            vibrationNumber = 0

# ---------------------------------------------
# Detect battery powered debouncing input signal
def detectMode():
    modes initial_mode = mode
    if mode == POWER_ON:
        for i in range(3):
            if digitalRead(PIN12V):
                mode = POWER_ON
                break
            mode = BATTERY
            delay(5)
    else:
        for i in range(3):
            if not digitalRead(PIN12V):
                mode = BATTERY
                break
            mode = POWER_ON
            delay(5)
    if initial_mode != mode:
        state = READ_GPS # Force update on mode change
        if DEBUG:
            Serial.print("Mode: ")
            Serial.println(mode)

#endif # RTC_H