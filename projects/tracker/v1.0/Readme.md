# GPS Tracker

This GPS trackers operates in 2 modes: Tracking and Keepalive. The battery charger may be connected which in terms of functionality is equivalent to the Tracking mode.
<br>
## Remote Configurations

Remote configurations are retreived from the server on every data upload. They are formated as a json string.

* Mode (0: Keepalive - 1: Tracking)
* Tint (Interval in minutes for data updates in Keepalive mode)
* TGPS (Maximum time for trying to get GPS location)
* Tsend (Maximum time for sending data to the server)

## Energy management

The energy saving features includes:

* Continuos battery voltage monitoring when awake
* Deep sleep between data updates
* Power off GPS antenna after GPS fixing
* Total awake time monitor

### Deep Sleep

* The tracker sleeps for 1h in Tracking mode or charging battery
* The tracker sleeps for Tint (min) in Keepalive mode (Typically 12h)
* The tracker sleeps for 24h when battery voltage is under 2.9V

<br>
### Wakeup handling

* The tracker wakes up always on charger connection
* If battery voltage is under 2.9V, the tracker sleeps for 24h immediately
* If the last data update was less than 1h before, the tracker sleeps for the remaining time to accomplish the 1h interval