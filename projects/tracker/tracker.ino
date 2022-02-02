#include "configs.h"
#include "settings.h"
#include "time.h"
#include "rtc.h"
#include "fsm_power_on.h"

hw_timer_t *timer = NULL;

Settings set_handler;
Sim7000 sim_device;
FSMpower fsm_power_on;

HardwareSerial fonaSS(1);
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

void IRAM_ATTR resetModule()
{
  ets_printf("reboot\n");
  esp_restart();
}

void detectMode()
{
  if (digitalRead(PIN12V))
  {
    mode = POWER_ON;
  }
  else
  {
    mode = BATTERY;
  }
}

void IRAM_ATTR isr()
{
  detectMode();
}

void setup()
{
  if (DEBUG)
  {
    // Initialize serial port
    Serial.begin(115200);
  }

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
  timer = timerBegin(0, 80, true);                 //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true); //attach callback
  timerAlarmWrite(timer, wdtTimeout, false);       //set time in us
  timerAlarmEnable(timer);
  // -------------------------------------

  // ------- Detect mode --------------
  pinMode(PIN12V, INPUT_PULLUP); // TODO this must be INPUT with an external pull down
  detectMode();
  attachInterrupt(PIN12V, isr, CHANGE);
  // -------------------------------------

  rtc_handle_wakeup();

  // Setup sim module
  sim_device.setup();
  
  fona.setFunctionality(1); // AT+CFUN=1

  // Setup FSM
  fsm_power_on.setup(&sim_device);

  // Setup FSM
  set_handler.setup();
}

void loop()
{
  float lat, lon, speed;
  char seq, mode, event, sats;
  uint16_t vbat, heading;

  seq = 10;
  mode = 1;
  event = 2;
  sats = 5;
  vbat = 4151;
  heading = 300;
  speed = 3.45;
  lat = 29.72939;
  lon = -95.64918;
  sprintf(msg, "%s,%d,%d,%d,%.5f,%.5f,%d,%d,%d,%d", "865235030717330", seq, mode, 0, lat, lon, (int)speed, heading, 6, vbat);
  Serial.print("len: ");
  Serial.println(strlen(msg));
  Serial.println(msg);

  if (sim_device.uploadData())
  {
    while (1)
      ;
  }
  timerWrite(timer, 0); //reset timer (feed watchdog)
  // time_t now;
  // time(&now);
  // Serial.println(now);
  delay(2000);
  // fsm_power_on.run();
}
