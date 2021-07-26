# VAL-1000 HTTP Server Firmware
## Getting Started with the VAL-1000

This firmware will get you started with spinning the VAL-1000 WiFi stepper board.

## Step 1 - Connect Power & Motor
1. Connect a 12V 2A+ power cord like [this](https://amzn.to/30qsbh7)
2. Connect a stepper motor like [this](https://amzn.to/3kUWkP4)

If you need to move something heavy, over 50 lbs, consider using a geared motor [like this](https://amzn.to/3c9xmI0)

## Step 2 - Download and open the Arduino code
Using Arduino, open the VAL-1000.ino file

You will need to install the ESP32 core, as well as several libraries:

Install the ESP32 core by following the instructions [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

Install the following libriaires inside Arduino
1. [TMC Stepper](https://www.arduino.cc/reference/en/libraries/tmcstepper/)
2. [AccelStepper](https://www.arduino.cc/reference/en/libraries/accelstepper/)

Install the following libraries outside Arduino. These cannot be installed using the Arduino library manager.
1. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
2. [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)  

a. Installing the ESPAsyncWebServer library

- [Click here](https://github.com/me-no-dev/ESPAsyncWebServer/archive/refs/heads/master.zip) to download the ESPAsyncWebServer library. You should have a .zip folder in your Downloads folder
- Unzip the .zip folder and you should get ESPAsyncWebServer-master folder
- Rename your folder from ESPAsyncWebServer-master to ESPAsyncWebServer
- Move the ESPAsyncWebServer folder to your Arduino IDE installation libraries folder

b. Installing the AsyncTCP Library

- The ESPAsyncWebServer library requires the AsyncTCP library to work. Follow the next steps to install that library:
- [Click here](https://github.com/me-no-dev/AsyncTCP/archive/refs/heads/master.zip) to download the ESPAsyncTCP library. You should have a .zip folder in your Downloads folder
- Unzip the .zip folder and you should get ESPAsyncTCP-master folder
- Rename your folder from AsyncTCP-master to AsyncTCP
- Move the ESPAsyncTCP folder to your Arduino IDE installation libraries folder

Finally, re-open your Arduino IDE. Compile the sketch and everything should compile.

## Step 3 - Upload the sketch
You do not need to change anything in the code.

Upload the firmware to your board.


## Step 4 - Connect to board

Using your smart phone or laptop (or any device with wifi), look for the wifi network name VALAR-AP

Connect to VALAR-AP

This is an access point which means you will connect directly to the device and will lose you internet connection.

When prompted, enter the wifi password "password". If you want to change this, you can modify the code in the API.h file for the variable ap_password.

You are now connected to the device and can begin to control the motor directly.
