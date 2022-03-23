/*  This is a simple example sketch to send or receive files via FTP protocol. The
 *  library supports FTP connect, GET/PUT, extended GET/PUT for larger files, rename,
 *  delete, and disconnect methods.
 *  
 *  NOTE: This code is still in progress!
 *  
 *  Should work on SIMCom modules that support FTP commands: SIM800/900/7000
 *  
 *  FTP Image Transfer Tutorial: https://github.com/botletics/SIM7000-LTE-Shield/wiki/FTP-Image-Transfer-Tutorial
 *  
 *  Author: Timothy Woo (www.botletics.com)
 *  Github: https://github.com/botletics/SIM7000-LTE-Shield
 *  Last Updated: 1/7/2021
 *  License: GNU GPL v3.0
 */

#include "Adafruit_FONA.h" // https://github.com/botletics/SIM7000-LTE-Shield/tree/master/Code
#include "SPIFFS.h"
File myFile;

const int CS_pin = 53;

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

// Define *one* of the following lines:
//#define SIMCOM_2G // SIM800/808/900/908, etc.
//#define SIMCOM_3G // SIM5320
#define SIMCOM_7000
//#define SIMCOM_7070
//#define SIMCOM_7500
//#define SIMCOM_7600

/************************* PIN DEFINITIONS *********************************/
// For botletics SIM7000 shield with ESP32
#define FONA_PWRKEY 4
#define FONA_RST 25
#define FONA_TX 18 // ESP32 hardware serial RX2 (GPIO16)
#define FONA_RX 19 // ESP32 hardware serial TX2 (GPIO17)

/************************* FTP SETTINGS *********************************/
#define serverIP    "ftp.drivehq.com" // Use global IP for remote connection
#define serverPort  21
#define username    "vladimir1284"
#define password    ""

// For ESP32 hardware serial
#include <HardwareSerial.h>
HardwareSerial fonaSS(1);
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
uint8_t type;
char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!

void setup() {
  Serial.begin(9600);
  Serial.println(F("*** FTP Example ***"));

  #ifdef LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
  #endif
  
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  fona.powerOn(FONA_PWRKEY); // Power on the module
  moduleSetup(); // Establishes first-time serial comm and prints IMEI

  // Set modem to full functionality
  fona.setFunctionality(1); // AT+CFUN=1
  fona.setNetworkSettings(F("hologram")); // For Hologram SIM card
  fona.setPreferredMode(38); // Use LTE only, not 2G
  fona.setPreferredLTEMode(1); // Use LTE CAT-M only, not NB-IoT

  // Connect to cell network and verify connection
  // If unsuccessful, keep retrying every 2s until a connection is made
  while (!netStatus()) {
    Serial.println(F("Failed to connect to cell network, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Connected to cell network!"));

  // Disable data connection before attempting to connect
  fona.enableGPRS(false);

  // Turn on data connection
  while (!fona.enableGPRS(true)) {
    Serial.println(F("Failed to enable data, retrying..."));
    delay(2000);
  }
  Serial.println(F("Enabled data!"));

  // Connect to FTP server
  Serial.println(F("Connecting to FTP server..."));
  while (!fona.FTP_Connect(serverIP, serverPort, username, password)) {
    Serial.println(F("Failed to connect to FTP server!"));
    delay(2000);
  }



  // Read the contents of a text file in the root directory of the FTP server
  // Make sure the file exists on your FTP server in the specified directory!
  Serial.println(F("Reading file from FTP server..."));

  char * readContent = fona.FTP_GET("hello_world.ino", "/", 1024);
  Serial.println(readContent); // DEBUG
  free(readContent); // Free up memory alloc

  // // Write the content we just read from the FTP file onto our local copy
  // if (!writeToFile("test.txt", readContent)) Serial.println(F("Failed to write to file!"));

  // Close FTP connection for good measure before performing next task
  if (!fona.FTP_Quit()) {
    Serial.println(F("Failed to close FTP connection!"));
  }

//   // Connect again to FTP server
//   Serial.println(F("Connecting to FTP server..."));
//   while (!fona.FTP_Connect(serverIP, serverPort, username, password)) {
//     Serial.println(F("Failed to connect to FTP server!"));
//     delay(2000);
//   }
  
//   // Upload text to a file in the server's root directory
//   const char* content = "This is an FTP test 123!"; // Text to put in the file. Could be from an SD card, etc.

//   // Upload a new file
//   Serial.println(F("Uploading file to FTP server..."));
//   if (!fona.FTP_PUT("upload.txt", "/", content, strlen(content))) { // File name, file path, content, content length
//     Serial.println(F("Failed to upload!"));
//   }

//   // Let's rename the file we just uploaded!
//   Serial.println(F("Renaming file on FTP server..."));
//   if (!fona.FTP_Rename("/", "upload.txt", "newFile.txt")) { // Path, old name, new name
//     Serial.println(F("Failed to change file name!"));
//   }

//   // You can also delete it if you want. I am leaving this commented
//   // out so you can check that it was really there to begin with.
//   // Uncomment then upload again and watch it disappear!
//   /*
//   Serial.println(F("Deleting file from FTP server..."));
//   if (!fona.FTP_Delete("newFile.txt", "/")) { // File name, file path
//     Serial.println(F("Failed to delete file!"));
//   }
//   */

//   // Get the timestamp of when the file we just renamed was last modified
// //  char timestamp[20];
//   uint16_t year;
//   uint8_t month, day, hour, minute, second;

//   Serial.println(F("Checking last modified timestamp..."));
//   if (!fona.FTP_MDTM("newFile.txt", "/", &year, &month, &day, &hour, &minute, &second)) {
//     Serial.println(F("Failed to get timestamp!"));
//   }
//   else {
//     Serial.print(F("Year: ")); Serial.println(year);
//     Serial.print(F("Month: ")); Serial.println(month);
//     Serial.print(F("Day: ")); Serial.println(day);
//     Serial.print(F("Hour: ")); Serial.println(hour);
//     Serial.print(F("Minute: ")); Serial.println(minute);
//     Serial.print(F("Second: ")); Serial.println(second);
//   }
  
//   // Now the really cool part! We're going to upload
//   // a picture to the server using the extended PUT
//   // method (auto-detected inside the FTP_PUT method
//   // based on the content size
//   // NOTE: Haven't tested extended PUT method yet because
//   // SIM7000G firmware does not support it for some reason...
//   /*
//   size_t fileSize;
//   char * uploadContent = readFromFile("test.png", &fileSize);

//   Serial.print("File size: "); Serial.print(fileSize); Serial.println(F(" bytes"));

//   // Upload picture via FTP
//   if (!fona.FTP_PUT("test.png", "/", uploadContent, fileSize)) { // File name, file path, content, content length
//     Serial.println(F("Failed to upload!"));
//   }
//   */

//   // Close FTP connection
//   // Note that with FTP GET/PUT requests, connection to FTP server is automatically
//   // closed after the request is successfully completed so the following function
//   // might give an error.
//   if (!fona.FTP_Quit()) {
//     Serial.println(F("Failed to close FTP connection!"));
//   }
}

void loop() {
  
}

void moduleSetup() {
  // SIM7000 takes about 3s to turn on and SIM7500 takes about 15s
  // Press Arduino reset button if the module is still turning on and the board doesn't find it.
  // When the module is on it should communicate right after pressing reset

  // Software serial:
  fonaSS.begin(115200); // Default SIM7000 shield baud rate

  Serial.println(F("Configuring to 9600 baud"));
  fonaSS.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  fonaSS.begin(9600);
  if (! fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    while (1); // Don't proceed if it couldn't find the device
  }

  // Hardware serial:
  /*
  fonaSerial->begin(115200); // Default SIM7000 baud rate

  if (! fona.begin(*fonaSerial)) {
    DEBUG_PRINTLN(F("Couldn't find SIM7000"));
  }
  */

  // The commented block of code below is an alternative that will find the module at 115200
  // Then switch it to 9600 without having to wait for the module to turn on and manually
  // press the reset button in order to establish communication. However, once the baud is set
  // this method will be much slower.
  /*
  fonaSerial->begin(115200); // Default LTE shield baud rate
  fona.begin(*fonaSerial); // Don't use if statement because an OK reply could be sent incorrectly at 115200 baud

  Serial.println(F("Configuring to 9600 baud"));
  fona.setBaudrate(9600); // Set to 9600 baud
  fonaSerial->begin(9600);
  if (!fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find modem"));
    while(1); // Don't proceed if it couldn't find the device
  }
  */

  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case SIM800L:
      Serial.println(F("SIM800L")); break;
    case SIM800H:
      Serial.println(F("SIM800H")); break;
    case SIM808_V1:
      Serial.println(F("SIM808 (v1)")); break;
    case SIM808_V2:
      Serial.println(F("SIM808 (v2)")); break;
    case SIM5320A:
      Serial.println(F("SIM5320A (American)")); break;
    case SIM5320E:
      Serial.println(F("SIM5320E (European)")); break;
    case SIM7000:
      Serial.println(F("SIM7000")); break;
    case SIM7070:
      Serial.println(F("SIM7070")); break;
    case SIM7500:
      Serial.println(F("SIM7500")); break;
    case SIM7600:
      Serial.println(F("SIM7600")); break;
    default:
      Serial.println(F("???")); break;
  }
  
  // Print module IMEI number.
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }

  // Needed for rare cases in which firmware on SIM7000 sets CFUN to 0 on start-up
  fona.setFunctionality(1); // Enable cellular (RF) with AT+CFUN=1
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
