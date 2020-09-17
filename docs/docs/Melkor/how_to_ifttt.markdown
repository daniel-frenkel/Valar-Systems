---
layout: page
title: IFTTT Guide
permalink: /Melkor/IFTTT
parent: Melkor
nav_order: 7
---

# How to add open and close commands to your Google Assistant, Alexa, or almost anything else.

The MorningRod does not yet have direct integration with Alexa and Google, however, you can use IFTTT to create a connection with almost any device out there.

This following method will use the MorningRod API to trigger the curtains. Here is a brief overview of how it works.

Each device has a unique token assigned to it. You can trigger your device to open and close by sending an HTTP request using this token. IFTTT works by triggering an HTTP request whenever a certain action is done.

## Opening the Curtains

1) Request your MorningRod auth token by pressing the "EMAIL" button on the MorningRod setup page  
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
19) Fill in the fields as below, but replace <b>both</b> “AUTH_TOKEN” fields with your authorization code from your email. Copy the URL below.  



```http://morningrod.blynk.cc/AUTH_TOKEN/update/V3?value=2```

![Webhooks settings](\images\IFTTT_fill_in.jpg "IFTTT Settings")


## Closing the Curtains

Do the same as above. But use this URL instead:
```http://morningrod.blynk.cc/AUTH_TOKEN/update/V3?value=1```

