# ESP32 Blynk Firmware Example
This ESP32 Arduino firmware will get you started with programming the VAL-1000 WiFi stepper board.

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
Set up the following pins to match those in the pin.h file

Segment Switch 
  V1
  1) Close
  2) Open
  3) Stop

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
