# VAL-1000 HTTP Server Firmware
## Getting Started with the VAL-1000

This firmware will get you started with spinning the VAL-1000 WiFi stepper board.

## Step 1 - Connect Power & Motor
1. Connect a 12V 2A+ power cord like [this](https://amzn.to/30qsbh7)
2. Connect a stepper motor like [this](https://amzn.to/3kUWkP4)

If you need to move something heavy, over 50 lbs, consider using a geared motor [like this](https://amzn.to/3c9xmI0)

## Step 2 - Download and open the Arduino code
You will need to install the ESP32 core, as well as several libraries:

Install the ESP32 core by following the instructions here

Install the following libriaires inside Arduino
1. TMC Stepper
2. AccelStepper

Install the following libraries outside Arduino
1)
2) 

https://reacoda.gitbook.io/molemi-iot/introducing-the-nodemcu/display-the-dht11-sensor-reading-on-a-web-server-using-nodemcu./installing-dht-library-on-the-esp8266/installing-the-asynchronous-web-server-library

Installing the ESPAsyncWebServer library

Click here to download the ESPAsyncWebServer library. You should have a .zip folder in your Downloads folder
Unzip the .zip folder and you should get ESPAsyncWebServer-master folder
Rename your folder from ESPAsyncWebServer-master to ESPAsyncWebServer
Move the ESPAsyncWebServer folder to your Arduino IDE installation libraries folder

Installing the ESPAsync TCP Library

The ESPAsyncWebServer library requires the ESPAsyncTCP library to work. Follow the next steps to install that library:
Click here to download the ESPAsyncTCP library. You should have a .zip folder in your Downloads folder
Unzip the .zip folder and you should get ESPAsyncTCP-master folder
Rename your folder from ESPAsyncTCP-master to ESPAsyncTCP
Move the ESPAsyncTCP folder to your Arduino IDE installation libraries folder

Finally, re-open your Arduino IDE.



https://github.com/me-no-dev/ESPAsyncWebServer/archive/refs/heads/master.zip
https://github.com/me-no-dev/AsyncTCP/archive/refs/heads/master.zip



## Step 3 - Download phone app
Download the Blynk app for iOS and Android

Create a new project with the ESP32.

You will receice an AUTH token in your email. Paste this token into blynk.h, replacing AUTH_TOKEN_HERE (keep the quotation marks).

Update your wifi SSID and password below the auto token (keep the quotes).

**NOTE:** Be sure 12V power is connected when uploading firmware.

Upload the firmware to the VAL-1000




## Step 4 - Set up app pins

Blynk works by sending values to the ESP32. These values are received by the ESP32 in the blynk.h file where you can see all of the different functions. 
For example, if you set up a button in Blynk and place it on Virtual pin 36, and tell it to go 0 to 1. 

Once the button is pressed in Blynk, it sends a "1" to V36, the function BLYNK_WRITE(V36) will execute. 

Set up the following pins to match those in the blynk.h file

Segment Switch 
  - Name: V1
  - Option 1: Close
  - Option 2: Open
  - Option 3: Stop

Horizonal Slider
  - Percent Open
  - V2
  - 0-100

Numeric Input
  - Max Distance
  - V22
  - 0-100
  - Step: 0.25

Numeric Input
  - Acceleration
  - V21
  - 0-9000
  - Step: 1

Numeric Input
  - Velocity
  - V34
  - 0-9000
  - Step: 1

Numeric Input
  - Current Close
  - V26
  - 0-2000
  - Step: 1
  
Numeric Input
  - Current Open
  - V25
  - 0-2000
  - Step: 1

Numeric Input
  - Stall Close
  - V124
  - 0-255
  - Step: 1

Numeric Input
  - Stall Open
  - V123
  - 0-255
  - Step: 1

Button
  - Reverse Motor Direction
  - V36
  - 0-1
  - MODE: SWITCH
  - OFF LABEL: 0
  - ON LABEL: 1

Button
  - Set Zero
  - V50
  - 0-1
  - MODE: PUSH
  - OFF LABEL: SET ZERO
  - ON LABEL: SET ZERO

Numeric Input
  - Stall Open
  - V125
  - 0-9000
  - Step: 1
  
  Press the play button up top, you should now be able to control the motor. 
