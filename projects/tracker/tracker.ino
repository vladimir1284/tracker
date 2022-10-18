#include "configs.h"
#include "settings.h"
#include "time.h"
#include "rtc.h"
#include "fsm_power_on.h"
#include "fsm_battery.h"

hw_timer_t *timer = NULL;

Settings set_handler;
Sim7000 sim_device;
FSMpower fsm_power_on;
FSMbattery fsm_battery;

HardwareSerial fonaSS(1);
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

void IRAM_ATTR isr()
{
  vibrationNumber++;
}

void IRAM_ATTR resetModule()
{
  ets_printf("reboot\n");
  esp_restart();
}

void setup()
{
  // Status LED
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED, HIGH);

  // Save battery asap if needed
  checkBatteryVoltage(false);

  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(921600); // 115200 in the serial monitor scaled x8
  }

  rtc_handle_wakeup();

  // ------- Reduce frequecy --------------
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
  // -------------------------------------

  // ------- watchdog begin --------------
  timer = timerBegin(0, 80, true);                 // timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true); // attach callback
  timerAlarmWrite(timer, wdtTimeout, false);       // set time in us
  timerAlarmEnable(timer);
  // -------------------------------------

  // ------- Detect mode --------------
  pinMode(PIN12V, INPUT);
  // -------------------------------------

  // ------- Detect movement --------------
  // attachInterrupt(PINVBR, isr, RISING);
  // -------------------------------------

  // Setup sim module
  sim_device.setup();

  // Setup FSM
  fsm_power_on.setup(&sim_device);
  fsm_battery.setup(&sim_device);

  // Setup settings handler
  set_handler.setup(&sim_device);
  set_handler.run();
  // Turn off modem
  // fona.setFunctionality(0); // AT+CFUN=0
}

void loop()
{
  // Serial.print(BUTTON_PIN_BITMASK);
  // rtc_sleep(10000000L);
  // timerWrite(timer, 0); //reset timer (feed watchdog)
  // // time_t now;
  // // time(&now);
  // // Serial.println(now);
  // delay(5000);

  detectMode();
  detectMovement();
  checkBatteryVoltage(true);
  // rtc_sleep(15*MIN_TO_uS_FACTOR);
  switch (mode)
  {
  case POWER_ON:
    fsm_power_on.run();
    break;

  case BATTERY:
    fsm_battery.run();
    break;

  default:
    break;
  }
}
