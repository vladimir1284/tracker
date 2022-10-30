# GPS Tracker

This GPS trackers operates in 2 modes: **Tracking** and **Keepalived**. The battery charger may be connected which in terms of functionality is equivalent to the **Tracking** mode.

## Remote Configurations

Remote configurations are retrieved from the server on every data upload. They are formatted as a json string.

* **Mode** (0: **Keepalived** \- 1: **Tracking**)
* **Tint** (Interval in minutes for **data updates** in Keepalived mode)
* **TGPS** (Maximum time for trying to get GPS location)
* **Tsend** (Maximum time for sending data to the server)

>{'Mode': 0, 'Tint': 720, 'TGPS': 5, 'Tsend': 3}

## Energy management

The energy saving features includes:

* Continuos battery voltage monitoring when awake
* Deep sleep between **data updates**
* Power off GPS antenna after GPS fixing
* Total awake time monitor

### Deep Sleep

* The tracker sleeps for **1h** in **Tracking** mode or **charging** battery
* The tracker sleeps for **Tint** (min) in **Keepalived** mode (Typically 12h)
* The tracker sleeps for **24h** when battery voltage is under **2.9V**
* The tracker sleeps for the corresponding amount of time if the total awake time exceeds **Tint+TGPS**

### Wake up handling

* The tracker wakes up always on charger connection
* If battery voltage is under **2.9V**, the tracker sleeps for **24h** immediately
* If the last **data update** was less than **1h** before, the tracker sleeps for the remaining time to accomplish the 1h interval

## Data upload

The data is uploaded to the server periodically depending on the operation mode. Every uploaded message has the following data:

* **imei** \- Sim7000 module IMEI
* **seq** \- Message sequence number\, incrementing in every data upload up to 255
* **charging** \- Whether the battery charger is connected
* **vbat** \- Battery voltage in mV
* **wur** \- Wake up reason \(ESP32\)
* **wdgc** \- Watchdog resets counter since the last wake up

### Keepalived

In **Keepalived** mode the GPS isn't powered and the location information is obtained from the cellular network

* **source** - LTE
* **mcc** - [Mobile country code](https://en.wikipedia.org/wiki/Mobile_country_code)
* **mnc** - [Mobile Network Code](https://en.wikipedia.org/?title=Mobile_Network_Code&redirect=no)
* **lac** - [Location area code](https://en.wikipedia.org/wiki/Location_area_identity)
* **cellid** - [GSM Cell ID](https://en.wikipedia.org/wiki/GSM_Cell_ID)

> // imei,seq,charging,vbat,wur,wdgc,LTE,mcc-mnc,lac,cellid
> msg\_lte = "865235032258663,0,1,4106,0,0,LTE,310-410,0x712A,137002000"

### Tracking (or charging)

In **Tracking** mode the GPS location is obtained within a boundary of **TGPS** minutes. If this time is exceeded, the location information is obtained from the cellular network and the LTE message is uploaded.

* **source** - LTE
* **latitude**
* **longitude**
* **speed** \- km/h
* **precision** - [Horizontal dilution of precision](https://en.wikipedia.org/wiki/Dilution_of_precision_%28navigation%29)

> // imei,seq,charging,vbat,wur,wdgc,GPS,latitude,longitude,speed,precision
> msg\_gps = "865235032258663,1,1,4106,0,0,GPS,41.64403,-108.54682,80,2"