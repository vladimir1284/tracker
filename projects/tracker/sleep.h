#if !defined(SLEEP_H)
#define SLEEP_H

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define RESET_PIN A3

bool sleeping;

// Disable hardware to consume battery energy
void disableHardware()
{
    power_spi_disable();    // Disable the Serial Peripheral Interface module.
    power_timer0_disable(); // Disable the Timer 0 module.
    power_timer1_disable(); // Disable the Timer 1 module.
    power_timer2_disable(); // Disable the Timer 2 module
    power_twi_disable();    // Disable the Two Wire Interface module.
    power_usart0_disable(); // Disable the USART 0 module.
    ADCSRA &= ~(1 << ADEN); // Ensure AD control register is disable before power disable
    power_adc_disable();    // Disable the Analog to Digital Converter module
}

// Enable hardware
void enableHardware()
{
    power_spi_enable();    // Enable the Serial Peripheral Interface module.
    power_timer0_enable(); // Enable the Timer 0 module..
    power_timer1_enable(); // Enable the Timer 1 module.
    power_timer2_enable(); // Enable the Timer 2 module
    power_twi_enable();    // Enable the Two Wire Interface module.
#ifdef SERIAL_PRINT
    power_usart0_enable(); // Enable the USART 0 module.
#endif
    power_adc_enable(); // Enable the Analog to Digital Converter module
}

void watchdogSetup()
{
    wdt_reset();
    /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  See table for time-out variations:
  WDP3 = 0 :For 1000ms Time-out
  WDP2 = 1 :For 1000ms Time-out
  WDP1 = 1 :For 1000ms Time-out
  WDP0 = 0 :For 1000ms Time-out
  */
    // Enter Watchdog Configuration mode:
    //  WDTCSR |= (1<<WDCE) | (1<<WDE);
    WDTCSR = bit(WDCE) | bit(WDE);
    // Set Watchdog settings:
    WDTCSR = bit(WDCE) | bit(WDE) | bit(WDIE) | bit(WDP3) | bit(WDP0); // set WDIE, and 8 seconds delay
    /* 
    WDTCSR = bit (WDCE) | bit (WDE) | bit (WDIE) | bit (WDP3);    // set WDIE, and 4 seconds delay
    WDTCSR = bit (WDCE) | bit (WDE) | bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0);    // set WDIE, and 2 seconds delay
    WDTCSR = bit (WDCE) | bit (WDE) | bit (WDIE) | bit (WDP2) | bit (WDP1);    // set WDIE, and 1 seconds delay
    */
}

void wdgSetup()
{
    /* disables all interrupts on the microcontroller so that 
    configuration is never disrupted and left unfinished */
    noInterrupts();

    // clear various "reset" flags
    MCUSR = 0;
    watchdogSetup();
    //sleep_bod_disable();

    // reenable interrupts
    interrupts();
}

// Disable hardware to consume battery energy
void watchdogBegin()
{
    //setup resetPin
    digitalWrite(RESET_PIN, HIGH);
    delay(200);
    pinMode(RESET_PIN, OUTPUT);

    wdgSetup();
}

void enterSleep(unsigned long sleep_ms)
{
    int nWakes;
    nWakes = (int)(sleep_ms / 8000);

    sleeping = true;
    sleep_enable();

    disableHardware();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    for (int i = 0; i < nWakes; i++)
    {
        wdgSetup();
        sleep_cpu();
        // wake up here
    }

    sleeping = false;
    sleep_disable();

    enableHardware();
}

// watchdog interrupt
ISR(WDT_vect)
{
    wdt_disable(); // disable watchdog
    digitalWrite(RESET_PIN, LOW);
} // end of WDT_vect

#endif // SLEEP_H