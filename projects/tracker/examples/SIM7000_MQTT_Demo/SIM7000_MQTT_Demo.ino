/*  This example sketch is for the Botletics SIM7000 shield and Arduino
 *  to collect GPS, temperature, and battery data and send those values via MQTT
 *  to just about any MQTT broker.
 *  
 *  NOTE: MQTTS development is in progress
 *  
 *  Just make sure to replace credentials with your own, and change the names of the
 *  topics you want to publish or subscribe to.
 *  
 *  Author: Timothy Woo (www.botletics.com)
 *  Github: https://github.com/botletics/SIM7000-LTE-Shield
 *  Last Updated: 1/7/2021
 *  License: GNU GPL v3.0
 */

#include "Adafruit_FONA.h" // https://github.com/botletics/SIM7000-LTE-Shield/tree/master/Code

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

/************************* PIN DEFINITIONS *********************************/
// For botletics SIM7000 shield with ESP32
#define FONA_PWRKEY 4
#define FONA_RST 5
#define FONA_TX 18 // ESP32 hardware serial RX2 (GPIO16)
#define FONA_RX 19 // ESP32 hardware serial TX2 (GPIO17)

// #define LED 13 // Just for testing if needed!

#define samplingRate 300 // The time we want to delay after each post (in seconds)

// For ESP32 hardware serial
#include <HardwareSerial.h>
HardwareSerial fonaSS(1);
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

/************************* MQTT PARAMETERS *********************************/
#define MQTT_SERVER      "test.mosquitto.org"
#define MQTT_PORT        1883
// #define MQTT_USERNAME    "MQTT_USERNAME"
// #define MQTT_PASSWORD    "MQTT_PASSWORD"

// Set topic names to publish and subscribe to
#define GPS_TOPIC       "864713037301317"
#define TEMP_TOPIC      "towit/temperature"
#define BATT_TOPIC      "towit/battery"
#define SUB_TOPIC       "towit/command"     // Subscribe topic name

// How many transmission failures in a row we're OK with before reset
uint8_t txfailures = 0;

/****************************** OTHER STUFF ***************************************/
// For temperature sensor
// #include <Wire.h>
// #include "Adafruit_MCP9808.h"

// // Create the MCP9808 temperature sensor object
// Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char imei[16] = {0}; // Use this for device ID
uint8_t type;
uint16_t battLevel = 0; // Battery level (percentage)
float latitude, longitude, speed_kph, heading, altitude, second;
uint16_t year;
uint8_t month, day, hour, minute;
uint8_t counter = 0;
//char PIN[5] = "1234"; // SIM card PIN

// NOTE: Keep the buffer sizes as small as possible, espeially on
// Arduino Uno which doesn't have much computing power to handle
// large buffers. On Arduino Mega you shouldn't have to worry much.
char latBuff[12], longBuff[12], locBuff[50], speedBuff[12],
     headBuff[12], altBuff[12], tempBuff[12], battBuff[12];

void setup() {
  Serial.begin(9600);
  Serial.println(F("*** SIM7000 MQTT Example ***"));

  #ifdef LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
  #endif
  
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  fona.powerOn(FONA_PWRKEY); // Power on the module
  moduleSetup(); // Establishes first-time serial comm and prints IMEI

  // tempsensor.wake(); // Wake up the MCP9808 if it was sleeping
  // if (!tempsensor.begin()) {
  //   Serial.println("Couldn't find the MCP9808!");
  //   while (1);
  // }

  // Unlock SIM card if needed
  // Remember to uncomment the "PIN" variable definition above
  /*
  if (!fona.unlockSIM(PIN)) {
    Serial.println(F("Failed to unlock SIM card"));
  }
  */

  // Set modem to full functionality
  fona.setFunctionality(1); // AT+CFUN=1

  // Configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  //fona.setNetworkSettings(F("your APN"), F("your username"), F("your password"));
  //fona.setNetworkSettings(F("m2m.com.attz")); // For AT&T IoT SIM card
  //fona.setNetworkSettings(F("telstra.internet")); // For Telstra (Australia) SIM card - CAT-M1 (Band 28)
  fona.setNetworkSettings(F("hologram")); // For Hologram SIM card

  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);


  fona.setPreferredMode(38); // Use LTE only, not 2G
  fona.setPreferredLTEMode(1); // Use LTE CAT-M only, not NB-IoT

  /*
  // Other examples of some things you can set:
  fona.setPreferredMode(38); // Use LTE only, not 2G
  fona.setPreferredLTEMode(1); // Use LTE CAT-M only, not NB-IoT
  fona.setOperatingBand("CAT-M", 12); // AT&T uses band 12
//  fona.setOperatingBand("CAT-M", 13); // Verizon uses band 13
  fona.enableRTC(true);
  
  fona.enableSleepMode(true);
  fona.set_eDRX(1, 4, "0010");
  fona.enablePSM(true);

  // Set the network status LED blinking pattern while connected to a network (see AT+SLEDS command)
  fona.setNetLED(true, 2, 64, 3000); // on/off, mode, timer_on, timer_off
  fona.setNetLED(false); // Disable network status LED
  */

  // Perform first-time GPS/data setup if the shield is going to remain on,
  // otherwise these won't be enabled in loop() and it won't work!
  // Enable GPS
  while (!fona.enableGPS(true)) {
    Serial.println(F("Failed to turn on GPS, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Turned on GPS!"));
}

void loop() {
  // Connect to cell network and verify connection
  // If unsuccessful, keep retrying every 2s until a connection is made
  while (!netStatus()) {
    Serial.println(F("Failed to connect to cell network, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Connected to cell network!"));

  // Disable data just to make sure it was actually off so that we can turn it on
//  fona.openWirelessConnection(false);

  // Open wireless connection if not already activated
  if (!fona.wirelessConnStatus()) {
    while (!fona.openWirelessConnection(true)) {
      Serial.println(F("Failed to enable connection, retrying..."));
      delay(2000); // Retry every 2s
    }
    Serial.println(F("Enabled data!"));
  }
  else {
    Serial.println(F("Data already enabled!"));
  }

  // Measure battery level
  battLevel = readVcc(); // Get voltage in mV

  // // Measure temperature
  // tempsensor.wake(); // Wake up the MCP9808 if it was sleeping
  // float tempC = tempsensor.readTempC();
  // float tempF = tempC * 9.0 / 5.0 + 32;
  // Serial.print("Temp: "); Serial.print(tempC); Serial.print("*C\t"); 
  // Serial.print(tempF); Serial.println("*F");
  
  // Serial.println("Shutting down the MCP9808...");
  // tempsensor.shutdown(); // In this mode the MCP9808 draws only about 0.1uA

  float temperature = 0.1*random(1,360); // Select what unit you want to use for this example

  // Turn on GPS if it wasn't on already (e.g., if the module wasn't turned off)
#ifdef turnOffShield
  while (!fona.enableGPS(true)) {
    Serial.println(F("Failed to turn on GPS, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Turned on GPS!"));
#endif

  // Get a fix on location, try every 2s
  // Use the top line if you want to parse UTC time data as well, the line below it if you don't care
//  while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude, &year, &month, &day, &hour, &minute, &second)) {
  // while (!fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude)) {
  //   Serial.println(F("Failed to get GPS location, retrying..."));
  //   delay(2000); // Retry every 2s
  // }
  // Serial.println(F("Found 'eeeeem!"));
  // Serial.println(F("---------------------"));
  // Serial.print(F("Latitude: ")); Serial.println(latitude, 6);
  // Serial.print(F("Longitude: ")); Serial.println(longitude, 6);
  // Serial.print(F("Speed: ")); Serial.println(speed_kph);
  // Serial.print(F("Heading: ")); Serial.println(heading);
  // Serial.print(F("Altitude: ")); Serial.println(altitude);
  // /*
  // // Uncomment this if you care about parsing UTC time
  // Serial.print(F("Year: ")); Serial.println(year);
  // Serial.print(F("Month: ")); Serial.println(month);
  // Serial.print(F("Day: ")); Serial.println(day);
  // Serial.print(F("Hour: ")); Serial.println(hour);
  // Serial.print(F("Minute: ")); Serial.println(minute);
  // Serial.print(F("Second: ")); Serial.println(second);
  // */
  // Serial.println(F("---------------------"));

  // // Format the floating point numbers
  // dtostrf(latitude, 1, 6, latBuff); // float_val, min_width, digits_after_decimal, char_buffer
  // dtostrf(longitude, 1, 6, longBuff);
  // dtostrf(speed_kph, 1, 0, speedBuff);
  // dtostrf(heading, 1, 0, headBuff);
  // dtostrf(altitude, 1, 1, altBuff);
  // dtostrf(temperature, 1, 2, tempBuff);
  // dtostrf(battLevel, 1, 0, battBuff);

  // // Construct a combined, comma-separated location array
  // sprintf(locBuff, "%s,%s,%s,%s", speedBuff, latBuff, longBuff, altBuff); // This could look like "10,33.123456,-85.123456,120.5"
  sprintf(locBuff, "%s,%s,%s,%s", "10", "33.123456", "-85.123456", "120.5"); // This could look like "10,33.123456,-85.123456,120.5"
  
  // If not already connected, connect to MQTT
  if (! fona.MQTT_connectionStatus()) {
    // Set up MQTT parameters (see MQTT app note for explanation of parameter values)
    fona.MQTT_setParameter("URL", MQTT_SERVER, MQTT_PORT);
    // Set up MQTT username and password if necessary
    // fona.MQTT_setParameter("USERNAME", MQTT_USERNAME);
    // fona.MQTT_setParameter("PASSWORD", MQTT_PASSWORD);
   fona.MQTT_setParameter("KEEPTIME", "600"); // Time to connect to server, 60s by default
   fona.MQTT_setParameter("RETAIN", "1"); // Time to connect to server, 60s by default
    
    Serial.println(F("Connecting to MQTT broker..."));
    if (! fona.MQTT_connect(true)) {
      Serial.println(F("Failed to connect to broker!"));
    }
  }
  else {
    Serial.println(F("Already connected to MQTT server!"));
  }

  // Now publish all the GPS and temperature data to their respective topics!
  // Parameters for MQTT_publish: Topic, message (0-512 bytes), message length, QoS (0-2), retain (0-1)
  if (!fona.MQTT_publish(GPS_TOPIC, locBuff, strlen(locBuff), 0, 1)) Serial.println(F("Failed to publish!")); // Send GPS location
  // if (!fona.MQTT_publish(TEMP_TOPIC, tempBuff, strlen(tempBuff), 1, 0)) Serial.println(F("Failed to publish!")); // Send temperature
  // if (!fona.MQTT_publish(BATT_TOPIC, battBuff, strlen(battBuff), 1, 0)) Serial.println(F("Failed to publish!")); // Send battery level

  // fona.MQTT_subscribe(SUB_TOPIC, 1); // Topic name, QoS
  
  // Unsubscribe to topics if wanted:
//  fona.MQTT_unsubscribe(SUB_TOPIC);

  // Enable MQTT data format to hex
//  fona.MQTT_dataFormatHex(true); // Input "false" to reverse

  // Disconnect from MQTT
//  fona.MQTT_connect(false);

  // Delay until next post
  Serial.print(F("Waiting for ")); Serial.print(samplingRate); Serial.println(F(" seconds\r\n"));
  delay(samplingRate * 1000UL); // Delay
}

void moduleSetup() {
  // SIM7000 takes about 3s to turn on and SIM7500 takes about 15s
  // Press Arduino reset button if the module is still turning on and the board doesn't find it.
  // When the module is on it should communicate right after pressing reset

  // Software serial:
  fonaSS.begin(115200, SERIAL_8N1, FONA_TX, FONA_RX); // baud rate, protocol, ESP32 RX pin, ESP32 TX pin

  Serial.println(F("Configuring to 9600 baud"));
  fonaSS.println("AT+IPR=9600");                    // Set baud rate
  delay(100);                                       // Short pause to let the command run
  fonaSS.begin(9600, SERIAL_8N1, FONA_TX, FONA_RX); // Switch to 9600
  if (!fona.begin(fonaSS))
  {
    Serial.println(F("Couldn't find FONA"));
    while (1)
      ; // Don't proceed if it couldn't find the device
  }

  type = fona.type();
  Serial.println(F("FONA is OK"));
  // Print module IMEI number.
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
}

// Read the module's power supply voltage
float readVcc() {
  // Read battery voltage
  if (!fona.getBattVoltage(&battLevel)) Serial.println(F("Failed to read batt"));
  else Serial.print(F("battery = ")); Serial.print(battLevel); Serial.println(F(" mV"));

  // Read LiPo battery percentage
//  if (!fona.getBattPercent(&battLevel)) Serial.println(F("Failed to read batt"));
//  else Serial.print(F("BAT % = ")); Serial.print(battLevel); Serial.println(F("%"));

  return battLevel;
}

bool netStatus() {
  int n = fona.getNetworkStatus();
  
  Serial.print(F("Network status ")); Serial.print(n); Serial.print(F(": "));
  if (n == 0) Serial.println(F("Not registered"));
  if (n == 1) Serial.println(F("Registered (home)"));
  if (n == 2) Serial.println(F("Not registered (searching)"));
  if (n == 3) Serial.println(F("Denied"));
  if (n == 4) Serial.println(F("Unknown"));
  if (n == 5) Serial.println(F("Registered roaming"));

  if (!(n == 1 || n == 5)) return false;
  else return true;
}
