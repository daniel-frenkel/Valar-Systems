
#define BOARD_FIRMWARE_VERSION        "4.2.1"

#define BOARD_NAME                    "MorningRod"        // Name of your product. Should match App Export request info.
#define BOARD_VENDOR                  "MorningRod"        // Name of your company. Should match App Export request info. 

#define PRODUCT_WIFI_SSID             "Valar"         // Name of the device, to be displayed during configuration. Should match export request info.
#define BOARD_CONFIG_AP_URL           "morningrod.com"     // Config page will be available in a browser at 'http://our-product.cc/'


#ifdef Model_D // Model D configuration

#define BOARD_HARDWARE_VERSION      "2.0.0"
#define BOARD_TEMPLATE_ID           "TMPL61618"            // ID of the Tile Template. Can be found in Tile Template Settings
#define BOARD_BUTTON_PIN            4                     // Pin where user button is attached
#define BOARD_BUTTON_ACTIVE_LOW     true                  // true if button is "active-low"

#define BOARD_LED_PIN               15                    // Set LED pin - if you have a single-color LED attached
#define BOARD_LED_INVERSE           false                 // true if LED is common anode, false if common cathode
#define BOARD_LED_BRIGHTNESS        1                    // 0..255 brightness control
#endif

#ifdef Model_W // Model W configuration

#define BOARD_HARDWARE_VERSION      "2.0.0"
#define BOARD_TEMPLATE_ID           "TMPL32917"            // ID of the Tile Template. Can be found in Tile Template Settings
#define BOARD_BUTTON_PIN            16                     // Pin where user button is attached
#define BOARD_BUTTON_ACTIVE_LOW     true                  // true if button is "active-low"

#define BOARD_LED_PIN               17                    // Set LED pin - if you have a single-color LED attached
#define BOARD_LED_INVERSE           false                 // true if LED is common anode, false if common cathode
#define BOARD_LED_BRIGHTNESS        1                    // 0..255 brightness control
#endif



/*
 * Advanced options
 */

#define BUTTON_HOLD_TIME_INDICATION   3000
#define BUTTON_HOLD_TIME_ACTION       10000

#define BOARD_PWM_MAX                 1023

#define LEDC_CHANNEL_1     1
#define LEDC_CHANNEL_2     2
#define LEDC_CHANNEL_3     3
#define LEDC_TIMER_BITS    10
#define LEDC_BASE_FREQ     12000

#define WIFI_NET_CONNECT_TIMEOUT      10000
#define WIFI_CLOUD_CONNECT_TIMEOUT    30000
#define WIFI_AP_CONFIG_PORT           80
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)
//#define WIFI_CAPTIVE_PORTAL_ENABLE

#define USE_TICKER

#define BLYNK_NO_DEFAULT_BANNER

#if defined(APP_DEBUG)
  #define DEBUG_PRINT(...) BLYNK_LOG1(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
#endif
