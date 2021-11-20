#if !defined(SETTINGS_H)
#define SETTINGS_H

#define UPLOAD_URL "http://trailerrental.pythonanywhere.com/towit/tracker_data/c0ntr453n1a/"
#define ID_URL "http://trailerrental.pythonanywhere.com/towit/tracker_id/c0ntr453n1a/"

// --------------- GSM ------------------
#define APN_NAME "nauta"

// --------------------------------------

// --------------- GPS ------------------

#define RXPinGPS 6
#define TXPinGPS 5
#define GPSBaud 9600

#define READ_GPS_DELAY 1000 // ms Time for continuosly collecting serial data from GPS module
#define iTGPS 10 // min
#define iHPOS 20 
#define iRADIUS 100 // m


// --------------------------------------




#endif // SETTINGS_H