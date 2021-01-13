---
layout: default
title: VALAR W1 - Window Opener
permalink: /valar_w1
nav_order: 2
has_children: false
---

# VALAR W1: Window Opener Setup Instructions

Use the Table of Content below to find the information you need. If starting new, begin at the top and work your way down, step by step.

{: .no_toc }

<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

---

## 1. Download the App

The Android or iOS app is required. Please download using the links below

[For Android](https://play.google.com/store/apps/details?id=cc.blynk.appexport.morning_rod)

[For iOS](https://apps.apple.com/al/app/morningrod/id1455653248)

Before mounting any hardware, we need to test the connection and the device.

Open up the app, create an account, and log in.

## 2. Reset the WiFi

During manufacturing, WiFi credentials may have been stored in your device. You need to clear them out before you can connect.

If you have an "Open Left" window opener, the clear WiFi button is located here:

![WiFi Reset Button Left](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Window_Opener_WiFi_Reset.jpg?v=1610127152)

If you have an "Open Right" window opener, the clear WiFi button is located here:

![WiFi Reset Button Right](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Window_Opener_WiFi_Reset_Right.jpg?v=1610127412)

Hold the button for **15 seconds** until the LED light begins flashing quickly 

The credentials are now cleared and we can connect to it.

## 3. Adding Device in App

![WIFI add device 1](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/small_Screenshot1.jpg?v=1610130491)

![WIFI add device 2](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Add_New_Device.jpg?v=1610131320)

**WARNING: Only 2.4Ghz networks will work. Your phone must be connected to a 2.4Ghz network for this to work**

### 3a. Initial Test

Before attaching the device to a window, we need to test the motor. 

To select a device, find it on the main page and **long press it**, holding it for 2 seconds

![WIFI add device 2](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Settings_Window_3.jpg?v=1610131256)

Select the settings tab

![WIFI add device 2](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Settings_Window_2.jpg?v=1610131256)


Be sure "DANGER: 1 CLOSE" is selected. We will learn more about this later

Select "FAST/LOUD" so it moves faster

**WARNING: Damage may occur if the magnets fell out during shipment. Ensure BOTH magnets are still in the carriage**

**NOTE: If the carriage does not stop on either end and damage begins, unplug immediately**


Check the bottom of the carriage for both magnets. They are attached to a black set screw. 

![WIFI add device 2](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/1_2.jpg?v=1610135325)


Use the purple buttons to move the opener to set it up.
    1) "ADJUST" will open the window 1 inch, this helps when setting up the magnet
    2) "CLOSE" will close to the end
    3) "OPEN" will open towards the motor housing

![WIFI add device 2](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Settings_Window1.jpg?v=1610131256)

Test out OPEN and CLOSE, ensure the carriage stops on its own on both ends




## 4. Attaching to the Window

We will cover attaching the window opener to your window and sill. The instruction only work if "DANGER: 1 CLOSE" is selected.

OPTIONAL: "DANGER: 1 CLOSE" refers to the direction of travel when closing the window. The window opener may be attached to a window that travels in either direction. If you look at the bottom of the unit, you will see a large "1" printed into the plastic. This is position 1. The opposite side, next to the motor is Position 2. If Position 1 is selected, that is the direction the carriage will travel in order to CLOSE your window. If Position 2 is selected, the window will close towards the motor.

You should try to use 1 CLOSE, but 2 CLOSE may also be used if required. **The following instructions only work for 1 CLOSE.**

Pay very close attention to the instructions, otherwise damage may occur if installed poorly.

**DO NOT REMOVE ADHESIVE UNTIL INSTRUCTED TO LATER ON**

You can purchase more 3M VHB tape if an error occurs. Here is what is attached to your device, but other VHB will work as well.
[McMaster-Carr Purchase Link](https://www.mcmaster.com/76675A21)

Quick overview video of what we will do next

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1-1iaJNHpKYRnrCVxs312azscdo9dG1Jn/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>

### 4a. Adjusting Sill Height

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1-ElJk9Vj7evdLrjtkDMWpiPavZiwAzXp/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>


### 4b. Proper Positioning 

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1-LXT9Bb7hntIYwuqFMs0nZXzBPFBcNk_/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>

### 4c. Frame Mount Setup

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1-ivxybllIJw1swqz3bM3C8B39uX40Fsw/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>

### 4d. Sill Mount Setup 

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1-NV6KbZa3SEOwbMl1Kw4MbjtRxVyIbw7/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>


**ALLOW THE ADHESIVE TO SET FOR 24 HOURS BEFORE USING IT!!!!!!!!!**




## 5. Current and Stall Setup

Each window requires a unique amount of force to move it. These next steps will set those parameters.

**WARNING:** Do NOT lock your window during these steps. If your windows Auto-Lock, keep them from locking. Damage may occur if locked.

**NOTE:** Changing speed from FAST to SLOW (or SLOW to FAST) will require resetting the current and stall parameters. 

The CrushGuard feature will only work if the stall values are perfect. When the motor is running, back EMF is produced. When resistance increases, so does the back EMF. The device measures this level of back EMF and automatically stops the motor once a threshold is reached. The stall value is the threshold value and must be accurate.

### 5a. Step 1 - Set Max Distance

Press the STEP 1 button. The device will automatically measure the exact distance of travel and set it for you. You may change this if required.

### 5b. Step 2 - Set Just-Open Position

Your windows has two levels of friction, which are called the "High-Friction" zone and "Low-Friction" zone. The High-Friction zone is the area within the pocket of the frame. The window experiences very high friction when in this zone. 

High-Friction Zone in Red. Low-Friction Zone in Green.

![High Friction Zone](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/High_Friction_Zone.jpg?v=1610154965)

You will be setting two current and stall values. One for the High and low zones. This way high current is only used in the High zone, allowing the motor to run quieter and cooler.

Watch this video to set the Just-Open position, which tells the window opener where to transition from High to Low settings. 

<figure class="video_container">
<iframe src="https://drive.google.com/file/d/1VCFY0jOTt8wXNtpZYoyxfePHxuSMJxg1/preview" width="800" height="450" frameborder="0" allowfullscreen="true"></iframe>
</figure>


### 5c. Step 3 - High Friction Current Calibration

Press the Run button to start the calibration. The device will automatically figure out the best value for the high friction zone.

### 5d. Step 4 - Low Friction Current Calibration

Press the Run button to start the calibration. The device will automatically figure out the best value for the low friction zone.

### 5e. Step 5 - High Friction Stall Calibration

Press the Run button to start the calibration. The device will automatically figure out the best value for the high friction zone.

Stall values range from -64 to 63. **Increasing** the value will **decrease** the sensitivity. 

If the device is still stalling when not required, increase this value by one until fixed.

If you place your finger in front of it and it does not stop, **decrease** the value in order to **increase** the sensitivity.

**NOTE:** Cold weather may increase resistance and require recalibration

### 5f. Step 6 - Low Friction Stall Calibration

Press the Run button to start the calibration. The device will automatically figure out the best value for the high friction zone.

If the device is still stalling when not required, increase this value by one until fixed. 

**NOTE:** Cold weather may increase resistance and require recalibration



## 6.  Integrating Google Assistant, Alexa, and More.

Valar devices to do not yet have direct integration with Alexa and Google, however, you can use IFTTT to create a connection with almost any device out there.

This following method will use the VALAR API to trigger the curtains. Here is a brief overview of how it works.

Each device has a unique token assigned to it. You can trigger your device to open and close by sending an HTTP request using this token. 

IFTTT works by triggering an HTTP request whenever a certain action is done.

### 6a. Opening the Window

1) Request your auth token by pressing the "EMAIL" button on the Valar device setup page  
2) Go to ifttt.com and create an account if you do not already have one  
3) Log in to ifttt.com  
4) If you have not already done so, add the IFTTT service to your Google assistant or Alexa device  
5) At top right search for Google assistant  
6) Click Google assistant connect  
7) Select your Google account  
8) Go to ‘My applets’  
9) Create a new applet  
10) Click the blue "+this" text  
11) In the search field type Google Assistant (or Alexa), press enter  
12) Select the Google assistant or Alexa trigger  
13) Google: Pick “Say a simple phrase”  
14) Click “Create Trigger”  
15) Click the blue +that  text  
16) In the search field type Webhooks  
17) Select Webhooks  
18) Click “Make a web request”  
19) Fill in the fields as below, but replace the “AUTH_TOKEN” field with your authorization code from your email. Copy the URL below.  

**The value parameter in the URL is the percentage of the window to open/close. Setting value=0 will close the window all the way, setting value=100 will open to 100%. Any value from 0-100 will work.**

```
http://morningrod.blynk.cc/AUTH_TOKEN/update/V23?value=100
```

![Webhooks settings](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Screenshot_20210113-080313.png?v=1610553935)

### 6b. Closing the Window

Do the same as above. But use this URL instead:
```
http://morningrod.blynk.cc/AUTH_TOKEN/update/V23?value=0
```


## 7. MQTT Setup

You can control your window locally via Home Assistant's MQTT Auto Discovery feature

![Learn More Here](https://www.home-assistant.io/docs/mqtt/discovery)

In the Valar App device settings, select the MQTT tab. 

**NOTE:** When entering values in the app, press the send button in the bottom right corner. Otherwise values won't save.


### 7a. Home Assistant Mosquitto Setup

Install the Mosquitto Add-On
https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md

**Create a username and password.** You must make one for this to work
Supervisor-->Mosquitto Broker-->Configuration
Add the login information. 

```
logins:
  - username: username
    password: password
```

It should look something like this:

```
logins:
  - username: username
    password: password
anonymous: false
customize:
  active: false
  folder: mosquitto
certfile: fullchain.pem
keyfile: privkey.pem
require_certificate: false
```


### 7b. VALAR App Setup
Under the MQTT tab of your device, enter the following:

Device Name: Enter the device name
Server IP Address: Enter your Home Assistant IP Address
MQTT Username: Enter the MQTT username
MQTT Password: Enter password you set up in Home Assistant
Prefix: Set as **homeassistant** in order to use Auto Discovery (recommended)

Press "START" button.

You will now see the MQTT Set Topic and State Topic to monitor in MQTT

Set topic allows you to control the device

State topic allows you to monitor the state of the device

### 7c. MQTT Commands

    1. Send **CLOSE** to close the window completely.
    2. Any digit from **1-100** will set the percent open. Send "50" to set 50% open. Send "100" to set 100% open.


### 7d. MQTT Testing

https://mqttfx.jensd.de/




## 8. SmartThings Integration Using WebCore

To integrate the Valar W1 window opener with Smart Things (henceforth “ST”), you will need to meet the following prerequisites:

1. Functional ST environment.
2. Basic understanding of the ST IDE.
3. WebCore installed and configured in ST, or a rules engine that can perform web get, puts, etc.
4. Optional: ST/Alexa/Google integration to perform voice commands.

The basic functionality will consist of: ST virtual switch, WebCore rule that opens and closes the W1 when the ST virtual switch is ON or OFF.  You may optionally create a 2nd switch that will open/close the window partially.

### 8a. Switch Creation
Log in to ST IDE, it will be something like https://graph-na02-useast1.api.smartthings.com/.  You will need to find your shard.  Use the ST forum for the process on finding your shard.

Go to My Devices, Select New Device.

1. Give the device a relative Name, for example “Open Kitchen Window”
2. Give the device a “Device Network Id”, this can be anything unique.  For example OKW1 (open kitchen window 1)
3. For Type, select “Simulated Switch”
4. Choose your Location, and Hub.
5. Hit Create.
6. Using the ST mobile app, go to SmartApps, WebCore, Settings, Available devices, Available devices, Which actuators, and select the newly available switch from step 1.

### 8b. WebCore rule
From the Valar app, select OTA, and under API TOKEN, select Email.  You will receive an email with your API key.

Log in to https://dashboard.webcore.co/

1. Select “New Piston” “</> Create a blank piston”
2. Select relative items for Author and Piston names.
3. Create the following statement (replace <API KEY> with API KEY you receive via email)
    a. if Open Kitchen Window's switch changes to on
        i. when true
            1.  do Make a GET request to https://morningrod.blynk.cc/<API KEY>/update/V23?value=100;
4.  Next create an if statement under else on the same level as if.
    a.  if Open Kitchen Window's switch changes to off
        i.      when true
            1. do Make a GET request to https://morningrod.blynk.cc/<API KEY>/update/V23?value=0;

It should look similar to this, only with your api key in the https request.
 


### 8c. Alexa integration
At this point, Alexa should have already found a switch called “Open Kitchen Window”.  If not run Alexa device discovery.

### 8d. Google integration
Perform device discovery (possible authorization of virtual switch device).

### 8e. Partially open window
Follow all of the same instructions above, giving the switch a unique name, ie “Breeze”.
For step “Webcore rule 3.1”  Use value=20 instead of value=100.
