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
<p>To learn more, please visit <a href=\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\">https://help.valarsystems.com</a></p>
<p>To add this device to your network go to http://192.168.4.1/wifi</p>
<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>
</div>
<br>



<div class="card">
<h2>Set Schedule</h2>
<form action="/schedule">
  <label for="timezone">Select Time Zone:</label>
  <select id="timezone" name="timezone">
  <option value="US/Pacific">US/Pacific</option>
  <option value="US/Mountain">US/Mountain</option>
  <option value="US/Central">US/Central</option>
  <option value="US/Eastern">US/Eastern</option>
  </select>
  <br>
  <br>
  
  <h3>Open Timer</h3>
  <div class="switch-field">
  <input type="radio" id="radio-one" name="open_timer" value="0" checked/>
  <label for="radio-one">OFF</label>
  <input type="radio" id="radio-two" name="open_timer" value="1"/>
  <label for="radio-two">ON</label>
  </div>
    

  <label for="open_time">Open Time: </label>
  <input type="time" id="open_time" name="open_time"><br>
  
  <br>
  
  <h3>Close Timer</h3>
  <div class="switch-field">
  <input type="radio" id="radio-one" name="close_timer" value="0" checked/>
  <label for="radio-one">OFF</label>
  <input type="radio" id="radio-two" name="close_timer" value="1"/>
  <label for="radio-two">ON</label>
  </div>
    
  <label for="close_time">Close Time: </label>
  <input type="time" id="close_time" name="close_time"><br>
  <br>
  
  <input type="submit" value="Save">
</form>
</div>


<br>
<div class="card">
<h2>Position Percent</h2>
<form action="/position">    
<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>
<label><b>move_to :</b></label>    
<input value = %PLACEHOLDER_PERCENT% type = "text" name = "move_percent">
<br/>
<input type="submit" value="Set Position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_percent=%PLACEHOLDER_PERCENT%</p>
</form>
</div>
<br>
<br>
<div class="card">
<h2>Set Zero Position</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="set_zero">
<input type="hidden" name="set_zero" value="1">
<input type="submit" value="Set Zero">
</form>
</div>
<br>
<br>
<div class="card">
<h2>Motor Parameters</h2>
<form action="/set_motor">
<h3>Max Steps</h3>
<p>Enter a value from 0-infinity. This is the maximum number of steps your motor will turn when set to 100 percent.</p>
<p>A negative value (e.g. -51200) will cause the motor to spin in the opposite direction.</p>
<p>One motor revolution has 200 full steps and 256 microsteps for a total of 51,200 steps per revolution.</p>
<p>To calculate this value, first measure the diameter of your motor pulley to calculate the distance travelled per revolution.</p>
<p>Example: If your GT2 pulley (20 teeth) diameter is 15mm, it has a circumfereance of 47.12mm. This means one revolution will cause your belt to move 47.12mm</p>
<p>Example: To move 1 meter, we take 1000mm/47.12 which is 21.22. This means we need 21.22 revolutions to move 1 meter. So 21.22 revolutions times 51,200 steps per revolution is 1086464. This is the max steps needed to move the motor by 1 meter. </p>
<label><b>max steps: </b></label>
<input value = "%PLACEHOLDER_MAX_STEPS%" type = "text" name = "max_steps">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_steps=%PLACEHOLDER_MAX_STEPS%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Speed Setting</h3>
<p>Enter a value from 10-200.</p>
<label><b>speed: </b></label>
<input value = "%PLACEHOLDER_MAX_SPEED%" type = "text" name = "max_speed">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_speed=%PLACEHOLDER_MAX_SPEED%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Current Setting</h3>
<p>Enter a value from 1-31.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_CURRENT%" type = "text" name = "current">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?current=%PLACEHOLDER_CURRENT%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Stall Setting</h3>
<p>Enter a stall value from -64 to 63</p>
<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for
readout. A lower value gives a higher sensitivity. Zero is
the starting value working with most motors.
<br>
-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL%" type = "text" name = "stall">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall=%PLACEHOLDER_STALL%</p>
<br>
<input type="submit" value="Save">
</form>
</div>
</body>
</html>
)=====";
