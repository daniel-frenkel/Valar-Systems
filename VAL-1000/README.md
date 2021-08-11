# VAL-1000 HTTP Server Firmware
## Getting Started with the VAL-1000

This firmware will get you started with spinning the VAL-1000 WiFi stepper board.

## Step 1 - Connect Power & Motor
1. Connect a 12V 2A+ power cord like [this](https://amzn.to/30qsbh7)
2. Connect a stepper motor like [this](https://amzn.to/3kUWkP4)

If you need to move something heavy, over 50 lbs, consider using a geared motor [like this](https://amzn.to/3c9xmI0)

## Step 2 - Open Browser on device with Wi-Fi
Use your phone, laptop, or other device with Wifi and open up a browser such as Chrome or Firefox.

Go to http://192.168.4.1

## Step 3 - Test

In the top box, enter a value from 0-100, the motor should spin.


## Step 3 - Setup

The position value is a percentage vate from 0-100. If you enter 100, it will move 100 percent of the way. If you enter 0, it will move to the home position.

The position percentage value is based on the max steps value. The max steps is the number of steps you want it to move to open 100 percent of the way. 

For example, if you have a curtain opener, and need it to move 20000 steps to open and close, you will enter 20000 here. 

There are 200 steps per revolution. To figure out how many steps to set, first measure the distance you want your device to travel. Let's say it's 20 inches, which is 508 mm.

Next, measure the diameter of the pulley you are using. In the case of the S1 curtain opener, it is 12mm.

Next, convert 12mm diameter to circumferance which is 37.7 mm.

Next, divide the total travel by cicumferance 


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
