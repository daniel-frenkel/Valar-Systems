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

![WIFI Reset Button Left](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Window_Opener_WiFi_Reset.jpg?v=1610127152)

If you have an "Open Right" window opener, the clear WiFi button is located here:

![WIFI Reset Button Right](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/Window_Opener_WiFi_Reset_Right.jpg?v=1610127412)

Hold the button for **15 seconds** until the LED light begins flashing quickly 

The credintials are now cleared and we can connect to it.

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

## 4. Attach to the Window

We will go over attaching the window opener to your window and sill.

Pay very close attention to these, otherwise damage may occur if installed poorly.

### 4a. Set Sill Height

### 4b. Set 

### 4c. Set 

### 4d. Set 

### 4e. Set 

### 4f. Set 

## 10. How to add open and close commands to your Google Assistant, Alexa, or almost anything else.

Valar devices to do not yet have direct integration with Alexa and Google, however, you can use IFTTT to create a connection with almost any device out there.

This following method will use the VALAR API to trigger the curtains. Here is a brief overview of how it works.

Each device has a unique token assigned to it. You can trigger your device to open and close by sending an HTTP request using this token. 

IFTTT works by triggering an HTTP request whenever a certain action is done.

### 10a. Opening the Window

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

**The value perameter in the URL is the percentage of the window to open/close. Setting value=0 will close the window all the way, setting value=100 will open to 100%. Any value from 0-100 will work.**

```http://morningrod.blynk.cc/AUTH_TOKEN/update/V23?value=100```

![Webhooks settings](\images\IFTTT_fill_in.JPG "IFTTT Settings")


### 10b. Closing the Window

Do the same as above. But use this URL instead:
```http://morningrod.blynk.cc/AUTH_TOKEN/update/V23?value=0```

