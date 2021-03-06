---
layout: default
title: VALAR D1 - Door Opener
permalink: /valar_d1
nav_order: 2
has_children: false
---

# VALAR R1: Curtain Opener
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

## STEP 1

Step 1

## STEP 2

Step 2


# How to add open and close commands to your Google Assistant, Alexa, or almost anything else.

Melkor does not yet have direct integration with Alexa and Google, however, you can use IFTTT to create a connection with almost any device out there.

This following method will use the VALAR API to trigger the curtains. Here is a brief overview of how it works.

Each device has a unique token assigned to it. You can trigger your device to open and close by sending an HTTP request using this token. IFTTT works by triggering an HTTP request whenever a certain action is done.

## Opening the Door

1) Request your Melkor auth token by pressing the "EMAIL" button on the Melkor setup page  
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



```http://morningrod.blynk.cc/AUTH_TOKEN/update/V3?value=2```

![Webhooks settings](\images\IFTTT_fill_in.JPG "IFTTT Settings")


## Closing the Door

Do the same as above. But use this URL instead:
```http://morningrod.blynk.cc/AUTH_TOKEN/update/V3?value=1```


## Setting Up The App

<p>Go to the Google Play or App store and download the MorningRod app.</p>

<p><a href="https://play.google.com/store/apps/details?id=cc.blynk.appexport.morning_rod&amp;hl=en" target="_blank" title="Google Play" rel="noopener noreferrer">Google Play</a></p>

<p>App Store </p>

<p>First, we need to reset the wifi configuration that is stored on the device. </p>

<p>To reset the WiFi configuration, use something small (such as the tip of a pen) to press and hold the TOP button for 12 seconds.</p>

<p>Next, open up the app and create an account.</p>

<p>Once created, log in. </p>

![App Settings](\images\app_setup1.jpg)

<p>If this is your first time setting up a device, this main screen will be empty. In this picture, there are already 3 devices set up.</p>

<p>Click the 3 lines in the upper right corner.</p>

![App Settings](\images\app_setup2.jpg)

Click on "Add new device"

![App Settings](\images\app_setup3.jpg)

<p>Choose a WiFi network ( Do NOT use a 5Ghz network), enter your password, and "Continue"</p>

![App Settings](\images\app_setup4.jpg)

<p>You will see this screen. If your device has an LED on it, it should be flashing blue.</p>

![App Settings](\images\app_setup5.jpg)

<p>Pick the device, it will be named MorningRod.</p>

<p><strong>If you do not see it, reset the WiFi settings on the device by pressing the button on TOP of the motor housing. Wait 20 seconds. This will erase the WiFi name and password that is stored in it.</strong></p>

![App Settings](\images\app_setup6.jpg)

<p>It will now take a minute to connect.</p>

![App Settings](\images\app_setup7.jpg)

<p>Once connected, give it a unique name.</p>

![App Settings](\images\app_setup8.jpg)

<p>It should now appear on your main screen.</p>

![App Settings](\images\app_setup9.jpg)

<p>You should now see your device on the main screen.</p>

<p>Now we need to tune the motors so they open and close properly. </p>


## Tuning The Motors

<p>Your MorningRod uses a very advanced stepper motor and driver to move the motor. </p>
<p>Instead of using a sensor to detect when the curtain has reached either end, it detects when the motor stalls by reaching the end. The motor driver measures the back EMF of the motor. When the curtains stall and cannot move, the back EMF sharply increases and the driver knows to shut it off.</p>
<p>We need to fine tune some settings to let the motor know when it should and should not be stalling. </p>
<p>Watch this video to get started.</p>
<p><iframe width="560" height="315" src="https://www.youtube.com/embed/eR2Ug87tsek" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen=""></iframe></p>
<p>First, let's set the speed. A speed between 80 and 120 is best. </p>
<p>Next, we need to set the stall values. These values change the sensitivity of the motors stall feature. </p>
<p>The reason to use the stall feature is so that the motors will stop moving if there is too much resistance. For example, if one of the curtains gets snagged on an object, you want the motor to stop so they don't rip everything off the wall.</p>
<p>To begin, set the open stall and close stall values to 0. </p>
<p>You can set them to a range of -64 to 63.</p>
<p>At -64, they are very sensitive and will stop the motor right away. In fact, it will be so sensitive that the motors will not even want to move.</p>
<p>At 63, the motor will NOT be sensitive and will NOT stall.</p>
<p>Starting at 0 is the best way to begin.</p>
<p>We will need to press either Open or Close to test the motor while setting up.</p>
<p>Manually move the carriages to the center of the rod and press OPEN.</p>
<p>If the motor does not move, set the OPEN value to 1.</p>
<p>If it still does not move, set it to 2. Continually increment the value up until it begins to move. </p>
<p> </p>
<p>Now we need to tune it to close. Do the same thing. Set the close stall to 0. If they do not move, increment 1 at a time. Chances are the best value will be around 20.</p>
