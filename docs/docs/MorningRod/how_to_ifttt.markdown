---
layout: page
title: IFTTT Guide
permalink: /MorningRod/IFTTT
parent: The MorningRod
nav_order: 7
---

# How to add open and close commands to your Google Assistant, Alexa, or almost anything else.

The MorningRod does not yet have direct integration with Alexa and Google, however, you can use IFTTT to create a connection with almost any device out there.

This following method will use the MorningRod API to trigger the curtains. Here is a brief overview of how it works.

Each device has a unique token assigned to it. You can trigger your device to open and close by sending an HTTP request using this token. IFTTT works by triggering an HTTP request whenever a certain action is done.

## Opening the Curtains

Request your MorningRod auth token by pressing the "EMAIL" button on the MorningRod setup page
Go to ifttt.com and create an account if you do not already have one
Log in to ifttt.com 
If you have not already done so, add the IFTTT service to your Google assistant or Alexa device
At top right search for Google assistant
Click Google assistant connect
Select your Google account
Go to ‘My applets’
Create a new applet
Click the blue "+this" text
In the search field type Google Assistant (or Alexa), press enter
Select the Google assistant or Alexa trigger
Google: Pick “Say a simple phrase”
Click “Create Trigger”
Click the blue +that  text
In the search field type Webhooks 
Select Webhooks
Click “Make a web request”

Fill in the fields as below, but replace “auth_token” with your authorization code from your email. Copy the URL below.

```http://morningrod.blynk.cc/auth_token/update/V13?value=1,http://morningrod.blynk.cc/auth_token/update/V13?value=0```

![Webhooks settings](https://cdn.shopify.com/s/files/1/0048/6244/3590/files/IFTTT_fill_in.png?463 "IFTTT Settings")

![Webhooks settings](\images\IFTTT_fill_in.png "IFTTT Settings")
![Webhooks settings](/docs/images/IFTTT_fill_in.png "IFTTT Settings")
![Webhooks settings](/images/IFTTT_fill_in.png "IFTTT Settings")


## Closing the Curtains

Do the same as above. But use this URL instead:
```http://morningrod.blynk.cc/auth_token/update/V12?value=1,http://morningrod.blynk.cc/auth_token/update/V12?value=0```

