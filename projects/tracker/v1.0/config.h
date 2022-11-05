#if !defined(CONFIG_H)
#define CONFIG_H

#define S_TO_uS_FACTOR 1000000ULL    /* Conversion factor for seconds to micro seconds */
#define MIN_TO_uS_FACTOR 60000000ULL // 60000000ULL /* Conversion factor for minutes to micro seconds */
#define MIN_TO_S_FACTOR 60           // 0 x0.1 for debug
#define TRACKING_INTERVAL 60         // Fixed time interval for charging or tracking updates

#define DEBUG true
#define SIM_PWR 5 // NRESET
#define PWRKEY 4  // GPIO4 -> RTC_GPIO10

#endif // CONFIG_H