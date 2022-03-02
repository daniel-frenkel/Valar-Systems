const char WIFI_HTML[] PROGMEM = R"=====(
 <!DOCTYPE html>
 <html lang="en">
   <style>
      .card{
      max-width: 350px;
      min-height: 250px;
      background: #02b875;
      padding: 30px;
      box-sizing: border-box;
      color: #FFF;
      margin: auto;
      box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
      }
   </style>
 <body>
 <div class="card">
 <h1>Enter Your WiFi Network Name and Password</h1>
 <form action=/set_wifi>
 <label class="label">Network Name</label><br>
 <input type = "text" name = "ssid">
 <br>
 <label>Network Password</label><br>
 <input type = "text" name = "pass">
 <br>
 <br>
 <input type="submit" value="Set Values">
 </form>
 </div>
 </body>
 </html>
)=====";

const char SETTINGS_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
 <html lang="en">
   <style>
      .card{
      max-width: 350px;
      min-height: 250px;
      background: #02b875;
      padding: 30px;
      box-sizing: border-box;
      color: #FFF;
      margin: auto;
      box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
      }
   </style>
 <body>
<div class="card">
<h2>Valar Systems</h2>
<h3>Motion Control</h3>
<p>To learn more, please visit <a href="https://help.valarsystems.com/docs/VAL-1000/VAL-1000">https://help.valarsystems.com</a></p>
<p>To add this device to your network go to <a href="http://192.168.4.1/wifi"> http://192.168.4.1/wifi</a></p>
<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>
</div>
<br>
<div class="card">
<h2>Position Percent</h2>
<form action="/position">    
<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>
<label><b>Percent Open :</b></label>    
<input value = %PLACEHOLDER_PERCENT% type = "number" name = "move_percent" min="0" max="100">
<br/>
<input type="submit" value="Set Position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_percent=%PLACEHOLDER_PERCENT%</p>
</form>
</div>
<br>
<br>
<div class="card">
<h2>Auto Calibrate</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="/calibrate">
<input type="hidden" name="calibrate" value="1">
<input type="submit" value="Calibrate">
</form>
</div>
<br>
<br>
<div class="card">
<h2>Motor Parameters</h2>
<form action="/set_motor">
<br>
<h3>Current Setting</h3>
<p>Enter a value from 400-2000. This is the amount of milliamps RMS the motor will draw.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_CURRENT%" type = "number" name = "current" min="400" max="2000">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?current=%PLACEHOLDER_CURRENT%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Stall Setting</h3>
<p>Enter a stall value from 0-255. The higher the value, the easier it will stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL%" type = "number" name = "stall" min="0" max="255">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall=%PLACEHOLDER_STALL%</p>
<br>
<input type="submit" value="Save">
</form>
</div>
</body>
</html>
)=====";
