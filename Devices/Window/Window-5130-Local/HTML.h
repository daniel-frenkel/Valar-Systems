//WIFI SETUP PAGE HTML

const char WIFI_HTML[] = " ";

/*
 <h1>Enter your home Wifi Name and Password</h1>
 <br> 
 <br> 
 <form action=/set_wifi>
 <label class="label">Network Name</label>
 <input type = "text" name = "ssid">
 <br/>
 <label>Network Password</label>
 <input type = "text" name = "pass">
 <br/>
 <input type="submit" value="Set Values">
 </form>
 */

//SETTINGS PAGE HTML

const char SETTINGS_HTML[] = " ";

/*
Enter the following text into https://tomeko.net/online_tools/cpp_text_escape.php?lang=en

<h2>Valar Systems</h2>
<h3>Motion Control</h3>
<p>To learn more, please visit <a href=\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\">https://help.valarsystems.com</a></p>
<p>To add this device to your network <a href=\"http://192.168.4.1/wifi\">go to http://192.168.4.1/wifi</a></p>
<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>
<br>
<h2>Position Percent</h2>
<form action="/position">    
<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>
<label><b>move_to :</b></label>    
<input value = %PLACEHOLDER_PERCENT% type = "text" name = "move_to_position">
<br/>
<input type="submit" value="Set Position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_to_position=%PLACEHOLDER_PERCENT%</p>
</form>

//REMOVE THIS
//TEMPLATE FOR OTHER SINGLE ACTIONS
<br>
<br>
<h2>Set Zero Position</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="set_zero">
<input type="hidden" name="set_zero" value="1">
<input type="submit" value="Set Zero">
</form>


<br>
<br>
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
<p>Enter a value from 10-200. This is the amount of RMS current the motor will draw.</p>
<label><b>speed: </b></label>
<input value = "%PLACEHOLDER_MAX_SPEED%" type = "text" name = "max_speed">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_speed=%PLACEHOLDER_MAX_SPEED%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Current Setting</h3>
<p>Enter a value from 400-2000. This is the amount of RMS current the motor will draw.</p>
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
*/
