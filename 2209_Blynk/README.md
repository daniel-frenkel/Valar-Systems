# VAL-1000 Firmware
## Getting Started with the TMC2209 and ESP32

This firmware will get you started with programming the VAL-1000 WiFi stepper board that I've created and is what I use in all of my Smart Home projects. 

Here are some use cases for this board:

- Automatic Curtain Opener
- Automatic Blind Opener
- Automatic Window Opener
- Automatic Door Opener
- Automatic Pet Feeder
- Motorized Camera Slider
- Automatic Drawer Opener
- And so much more

If you want to put a safe and highly advanced motor into something, I recommend you use this. It is impossible to burn out, easy to work with, ultra-quite, and extremely safe. StallGuard will prevent your fingers from being crushed.

- You can get the VAL-1000 from my [website](https://valarsystems.com/products/val1000-all-in-one-motion-controller)
- Or you can get it from [Tindie](https://www.tindie.com/products/valar/wifi-stepper-motor-board/)

You can also use your own ESP32 and TMC2209 stepper driver.

The VAL-1000 is an ultra-compact wifi-controlled stepper driver that uses an ESP32 and TMC2209.

Setup is super easy. Plug in a 12V 2A+ power cord from Amazon or anywhere else, connect the 4 wires of your stepper motor, download the app, and start moving your motor!

The TMC2209 is REVOLUTIONARY! It is literally the best stepper driver out there because:
  1) StallGuard. It's like magic.
  2) The price is incredibly good.
  3) The current is very high.
  4) It is EXTREMEMLY quiet. At low speeds you only hear the motor bearings move.

![PCB IMAGE](/docs/images/PCB_github.jpg)
![PCB IMAGE2](/docs/images/PCB_Details.jpg)


## Step 1 - Download and open the Arduino code
You may need to install several libraries:
1. TMC Stepper
2. AccelStepper
3. Blynk

## Step 2 - Download phone app
Download the Blynk app for iOS and Android

Create a new project with the ESP32.

You will receice an AUTH token in your email. Paste this token into blynk.h, replacing AUTH_TOKEN_HERE (keep the quotation marks).

Update your wifi SSID and password below the auto token (keep the quotes).

**NOTE:** Be sure 12V power is connected when uploading firmware.

Upload the firmware to the VAL-1000




## Step 3 - Set up app pins

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
