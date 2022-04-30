
# --------------- watchdog ------------------

WDT_TIMEOUT	= 60000000 #time in us to trigger the watchdog

# --------------------------------------

# --------------- SIM ------------------

MSG_SIZE	= 128
APN_NAME	= "hologram"

RETRIES	= 30
CREDIT_INIT	= 7
CREDIT_LEN	= 6

FONA_RX	= 19
FONA_TX	= 18
SIM_PWR	= 25
PWRKEY	= 4 # GPIO4 -> RTC_GPIO10

# --------------------------------------

# --------------- SERVER PARAMETERS -----------------
SERVER	= "http://trailerrental.pythonanywhere.com"
ADDR	= "/towit/tracker_data"

# --------------------------------------

# --------------- initial Configs ------------------

iMAX_ERRORS	= 3

# On Battery
iTintB	= 720 # min
iTsendB	= 10 # min
iTGPSB	= 10  # min
iSMART	= True

# On Power connected
iTGPS	= 10  # min
iTint	= 60  # min
iTsend	= 3  # min

# --------------------------------------

# --------------- FSM ------------------

S_TO_uS_FACTOR	= 1000000    # Conversion factor for seconds to micro seconds 
MIN_TO_uS_FACTOR	= 60000000 #60000000 /* Conversion factor for minutes to micro seconds */
MIN_TO_S_FACTOR	= 60            # 0 x0.1 for debug
PIN12V	= 34                    # Input pin for checking 12V connection
PWR_PIN_BITMASK	= 0x400000000 # 2^PIN12V 0b100,00000000,00000000,00000000,00000000

# enum modes
# # Power connected
#     POWER_ON,
#     BATTERY

# enum states
# # Power connected
#     IDLE,
#     READ_GPS,
#     SEND_DATA,
#     ERROR

# --------------------------------------

# --------------- EEPROM ADDRESSES ------------------

EEPROM_SIZE	= 32

EEPROM_KEY = 0x89 # used to identify if valid data in EEPROM

KEY_ADDR = 0        # the EEPROM address used to store the ID
# int Tcheck_ADDR = 1;     // the EEPROM address used to store Tcheck
MAX_ERRORS_ADDR = 2 # the EEPROM address used to store MAX_ERRORS
Tint_ADDR = 3       # the EEPROM address used to store Tint (2 bytes)
TintB_ADDR = 5      # the EEPROM address used to store TintB (2 bytes)
TGPS_ADDR = 7       # the EEPROM address used to store TGPS
TGPSB_ADDR = 8      # the EEPROM address used to store TGPSB
SMART_ADDR = 9      # the EEPROM address used to store SMART
Tsend_ADDR = 10     # the EEPROM address used to store Tsend
TsendB_ADDR = 11    # the EEPROM address used to store TsendB
trackerID_ADDR = 12 # the EEPROM address used to store trackerID (2 bytes)
# int any = 14;    // the EEPROM address used to store

# --------------------------------------

# --------------- Handle Dynamic Configs ------------------
MAX_INPUT_LENGTH	= 200

# Error codes
WRONG_PASS	= 200
WRONG_ID	= 201

# --------------------------------------

# --------------- Vibration sensor ------------------
MOVTHRESHOLD	= 1
MOVDELAY	= 10
PINVBR	= 33
VBR_PIN_BITMASK	= 0x200000000 # 2^PINVBR 0b10,00000000,00000000,00000000,00000000

# --------------------------------------
