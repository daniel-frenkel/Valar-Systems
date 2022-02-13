//WIFI SETUP PAGE HTML

const char WIFI_HTML[] = "<h1>Enter your home Wifi Name and Password</h1>\n <br> \n <br> \n <form action=/set_wifi>\n <label class=\"label\">Network Name</label>\n <input type = \"text\" name = \"ssid\">\n <br/>\n <label>Network Password</label>\n <input type = \"text\" name = \"pass\">\n <br/>\n <input type=\"submit\" value=\"Set Values\">\n </form>";

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

const char SETTINGS_HTML[] = "<h2>Valar Systems</h2>\n<h3>Motion Control</h3>\n<p>To learn more, please visit <a href=\\\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\\\">https://help.valarsystems.com</a></p>\n<p>To add this device to your network <a href=\\\"http://192.168.4.1/wifi\\\">go to http://192.168.4.1/wifi</a></p>\n<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>\n<br>\n<h2>Position Percent</h2>\n<form action=\"/position\">    \n<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>\n<label><b>move_to :</b></label>    \n<input value = %PLACEHOLDER_PERCENT% type = \"text\" name = \"move_to_position\">\n<br/>\n<input type=\"submit\" value=\"Set Position\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_to_position=%PLACEHOLDER_PERCENT%</p>\n</form>\n<br>\n<h2>Auto Tune Motor</h2>\n<p>Start the automatic tuning sequence</p>\n<p>UNLOCK YOUR WINDOW and clear all obstacles before starting.</p>\n<form action=\"auto_tune\">\n<input type=\"hidden\" name=\"auto_tune\" value=\"1\">\n<input type=\"submit\" value=\"Auto Tune\">\n</form>\n<br>\n<br>\n<h2>Manually Set Motor Parameters</h2>\n<form action=\"/set_motor\">\n<br>\n<h3>Close Position Setting</h3>\n<p>The changes which end of the device the window will close to. \n<p>Enter a value of 1 or 2.\n<br>\n<label><b>close position: </b></label>\n<input value = \"%PLACEHOLDER_CLOSE_POSITION%\" type = \"text\" name = \"close_position\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_position=%PLACEHOLDER_CLOSE_POSITION%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<h3>Speed Setting</h3>\n<p>Enter a value of 0 or 1.\n<br>\n0 = SLOW/SILENT\n<br>\n1 = FAST/LOUD</p>\n<label><b>speed: </b></label>\n<input value = \"%PLACEHOLDER_SPEED%\" type = \"text\" name = \"fast_loud\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?fast_loud=%PLACEHOLDER_SPEED%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h2>Current Settings</h2>\n<p>There are 2 zones that the current changes in. The \"high\" zone is within 1 inch of the window closing. The current increases in this zone due to the increased friction required to close the window all the way. The \"low\" current zone is the remaining part of the window.</p>\n<br>\n<h3>Current Open High Setting</h3>\n<p>Enter a value from 1-26 to set this current.</p>\n<label><b>current: </b></label>\n<input value = \"%PLACEHOLDER_OPEN_CURRENT_HIGH%\" type = \"text\" name = \"open_current_high\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?open_current_high=%PLACEHOLDER_OPEN_CURRENT_HIGH%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Current Open Low Setting</h3>\n<p>Enter a value from 1-26 to set this current.</p>\n<label><b>current: </b></label>\n<input value = \"%PLACEHOLDER_OPEN_CURRENT_LOW%\" type = \"text\" name = \"open_current_low\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?open_current_low=%PLACEHOLDER_OPEN_CURRENT_LOW%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Current Open High Setting</h3>\n<p>Enter a value from 1-26 to set this current.</p>\n<label><b>current: </b></label>\n<input value = \"%PLACEHOLDER_CLOSE_CURRENT_HIGH%\" type = \"text\" name = \"close_current_high\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_current_high=%PLACEHOLDER_CLOSE_CURRENT_HIGH%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Current Open High Setting</h3>\n<p>Enter a value from 1-26 to set this current.</p>\n<label><b>current: </b></label>\n<input value = \"%PLACEHOLDER_CLOSE_CURRENT_LOW%\" type = \"text\" name = \"close_current_low\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_current_low=%PLACEHOLDER_CLOSE_CURRENT_LOW%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h2>Stall Settings</h2>\n<br>\n<h3>Stall Open High Setting</h3>\n<p>Enter a stall value from -64 to 63</p>\n<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for\nreadout. A lower value gives a higher sensitivity. Zero is\nthe starting value working with most motors.\n<br>\n-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>\n<label><b>stall: </b></label>\n<input value = \"%PLACEHOLDER_STALL_OPEN_HIGH%\" type = \"text\" name = \"stall_open_high\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_open_high=%PLACEHOLDER_STALL_OPEN_HIGH%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Stall Open Low Setting</h3>\n<p>Enter a stall value from -64 to 63</p>\n<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for\nreadout. A lower value gives a higher sensitivity. Zero is\nthe starting value working with most motors.\n<br>\n-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>\n<label><b>stall: </b></label>\n<input value = \"%PLACEHOLDER_STALL_OPEN_LOW%\" type = \"text\" name = \"stall_open_low\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_open_low=%PLACEHOLDER_STALL_OPEN_LOW%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Stall Close High Setting</h3>\n<p>Enter a stall value from -64 to 63</p>\n<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for\nreadout. A lower value gives a higher sensitivity. Zero is\nthe starting value working with most motors.\n<br>\n-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>\n<label><b>stall: </b></label>\n<input value = \"%PLACEHOLDER_STALL_CLOSE_HIGH%\" type = \"text\" name = \"stall_close_high\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_close_high=%PLACEHOLDER_STALL_CLOSE_HIGH%</p>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<h3>Stall Close Low Setting</h3>\n<p>Enter a stall value from -64 to 63</p>\n<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for\nreadout. A lower value gives a higher sensitivity. Zero is\nthe starting value working with most motors.\n<br>\n-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>\n<label><b>stall: </b></label>\n<input value = \"%PLACEHOLDER_STALL_CLOSE_LOW%\" type = \"text\" name = \"stall_close_low\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_close_low=%PLACEHOLDER_STALL_CLOSE_LOW%</p>\n<br>\n<input type=\"submit\" value=\"Save\">\n<br>\n<br>\n<br>\n<h3>Max Steps</h3>\n<h2>DO NOT ADJUST UNLESS YOU KNOW WHAT YOU'RE DOING</h2>\n<p>Enter a value from 0-infinity. This is the maximum number of steps your motor will turn when set to 100 percent.</p>\n<p>A negative value (e.g. -51200) will cause the motor to spin in the opposite direction.</p>\n<p>One motor revolution has 200 full steps and 256 microsteps for a total of 51,200 steps per revolution.</p>\n<p>To calculate this value, first measure the diameter of your motor pulley to calculate the distance travelled per revolution.</p>\n<p>Example: If your GT2 pulley (20 teeth) diameter is 15mm, it has a circumfereance of 47.12mm. This means one revolution will cause your belt to move 47.12mm</p>\n<p>Example: To move 1 meter, we take 1000mm/47.12 which is 21.22. This means we need 21.22 revolutions to move 1 meter. So 21.22 revolutions times 51,200 steps per revolution is 1086464. This is the max steps needed to move the motor by 1 meter. </p>\n<label><b>max steps: </b></label>\n<input value = \"%PLACEHOLDER_MAX_STEPS%\" type = \"text\" name = \"max_steps\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_steps=%PLACEHOLDER_MAX_STEPS%</p>\n<input type=\"submit\" value=\"Save\">\n</form>\n<br>\n<br>\n<br>\n<h1>Motor Adjustments</h1>\n<br>\n<h2>Position Open</h2>\n<p>Press this button to set the home position of your motor to zero</p>\n<form action=\"position_open\">\n<input type=\"hidden\" name=\"position_open\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Position Close</h2>\n<p>Press this button to set the home position of your motor to zero</p>\n<form action=\"position_close\">\n<input type=\"hidden\" name=\"position_close\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Position Adjust</h2>\n<p>Press this button to set the home position of your motor to zero</p>\n<form action=\"position_adjust\">\n<input type=\"hidden\" name=\"position_adjust\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Step 1 Calibration</h2>\n<p>Press this button to automatically set the step count</p>\n<form action=\"step_1\">\n<input type=\"hidden\" name=\"step_1\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Step 3 Calibration</h2>\n<p>Press this button to automatically calibrate the HIGH current settings</p>\n<form action=\"step_3\">\n<input type=\"hidden\" name=\"step_3\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Step 4 Calibration</h2>\n<p>Press this button to automatically calibrate the LOW current setting</p>\n<form action=\"step_4\">\n<input type=\"hidden\" name=\"step_4\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Step 5 Calibration</h2>\n<p>Press this button to automatically calibrate the LOW stall setting</p>\n<form action=\"step_5\">\n<input type=\"hidden\" name=\"step_5\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>\n<br>\n<br>\n<h2>Step 6 Calibration</h2>\n<p>Press this button to automatically calibrate the LOW stall setting</p>\n<form action=\"step_6\">\n<input type=\"hidden\" name=\"step_6\" value=\"1\">\n<input type=\"submit\" value=\"Start\">\n</form>";

/*
Enter the following text into https://tomeko.net/online_tools/cpp_text_escape.php?lang=en

<h2>Valar Systems</h2>
<h3>Motion Control</h3>
<p>To learn more, please visit <a href=\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\">https://help.valarsystems.com</a></p>
<p>To add this device to your network <a href=\"http://192.168.4.1/wifi\">go to http://192.168.4.1/wifi</a></p>
<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>
<br>
<h2>How to set up the motor</h2>
<ol>
  <li>Set Close Position</li>
  <li>Set Speed</li>
  <li>Auto Tune Motor</li>
</ol>

<h3>Step 1</h3>
<p>The window opener is able to close in either direction. This means that you can close your window away from the motor, or you can close it toward your motor</p>
<p>To properly set the location of closing, find a magnet somewhere in your house. The stonger the better.</p>

<h3>Step 2</h3>

<h3>Step 3</h3>

<h2>Position Percent</h2>
<form action="/position">    
<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>
<label><b>move_to :</b></label>    
<input value = %PLACEHOLDER_PERCENT% type = "text" name = "move_to_position">
<br/>
<input type="submit" value="Set Position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_to_position=%PLACEHOLDER_PERCENT%</p>
</form>
<br>
<h2>Auto Tune Motor</h2>
<p>Start the automatic tuning sequence</p>
<p>UNLOCK YOUR WINDOW and clear all obstacles before starting.</p>
<form action="auto_tune">
<input type="hidden" name="auto_tune" value="1">
<input type="submit" value="Auto Tune">
</form>
<br>
<br>
<h2>Manually Set Motor Parameters</h2>
<form action="/set_motor">
<br>
<h3>Close Position Setting</h3>
<p>The changes which end of the device the window will close to. 
<p>Enter a value of 1 or 2.
<br>
<label><b>close position: </b></label>
<input value = "%PLACEHOLDER_CLOSE_POSITION%" type = "text" name = "close_position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_position=%PLACEHOLDER_CLOSE_POSITION%</p>
<input type="submit" value="Save">
<br>
<h3>Speed Setting</h3>
<p>Enter a value of 0 or 1.
<br>
0 = SLOW/SILENT
<br>
1 = FAST/LOUD</p>
<label><b>speed: </b></label>
<input value = "%PLACEHOLDER_SPEED%" type = "text" name = "fast_loud">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?fast_loud=%PLACEHOLDER_SPEED%</p>
<input type="submit" value="Save">
<br>
<br>
<h2>Current Settings</h2>
<p>There are 2 zones that the current changes in. The "high" zone is within 1 inch of the window closing. The current increases in this zone due to the increased friction required to close the window all the way. The "low" current zone is the remaining part of the window.</p>
<br>
<h3>Current Open High Setting</h3>
<p>Enter a value from 1-26 to set this current.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_OPEN_CURRENT_HIGH%" type = "text" name = "open_current_high">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?open_current_high=%PLACEHOLDER_OPEN_CURRENT_HIGH%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Current Open Low Setting</h3>
<p>Enter a value from 1-26 to set this current.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_OPEN_CURRENT_LOW%" type = "text" name = "open_current_low">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?open_current_low=%PLACEHOLDER_OPEN_CURRENT_LOW%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Current Close High Setting</h3>
<p>Enter a value from 1-26 to set this current.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_CLOSE_CURRENT_HIGH%" type = "text" name = "close_current_high">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_current_high=%PLACEHOLDER_CLOSE_CURRENT_HIGH%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Current Close Low Setting</h3>
<p>Enter a value from 1-26 to set this current.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_CLOSE_CURRENT_LOW%" type = "text" name = "close_current_low">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?close_current_low=%PLACEHOLDER_CLOSE_CURRENT_LOW%</p>
<input type="submit" value="Save">
<br>
<br>
<h2>Stall Settings</h2>
<br>
<h3>Stall Open High Setting</h3>
<p>Enter a stall value from -64 to 63</p>
<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for
readout. A lower value gives a higher sensitivity. Zero is
the starting value working with most motors.
<br>
-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL_OPEN_HIGH%" type = "text" name = "stall_open_high">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_open_high=%PLACEHOLDER_STALL_OPEN_HIGH%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Stall Open Low Setting</h3>
<p>Enter a stall value from -64 to 63</p>
<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for
readout. A lower value gives a higher sensitivity. Zero is
the starting value working with most motors.
<br>
-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL_OPEN_LOW%" type = "text" name = "stall_open_low">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_open_low=%PLACEHOLDER_STALL_OPEN_LOW%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Stall Close High Setting</h3>
<p>Enter a stall value from -64 to 63</p>
<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for
readout. A lower value gives a higher sensitivity. Zero is
the starting value working with most motors.
<br>
-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL_CLOSE_HIGH%" type = "text" name = "stall_close_high">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_close_high=%PLACEHOLDER_STALL_CLOSE_HIGH%</p>
<input type="submit" value="Save">
<br>
<br>
<h3>Stall Close Low Setting</h3>
<p>Enter a stall value from -64 to 63</p>
<p>This signed value controls StallGuard2 level for stall output and sets the optimum measurement range for
readout. A lower value gives a higher sensitivity. Zero is
the starting value working with most motors.
<br>
-64 to +63: A higher value makes StallGuard2 less sensitive and requires more torque to indicate a stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL_CLOSE_LOW%" type = "text" name = "stall_close_low">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall_close_low=%PLACEHOLDER_STALL_CLOSE_LOW%</p>
<br>
<input type="submit" value="Save">
<br>
<br>
<br>
<h3>Max Steps</h3>
<h2>DO NOT ADJUST UNLESS YOU KNOW WHAT YOU'RE DOING</h2>
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
</form>
<br>
<br>
<br>
<h1>Motor Adjustments</h1>
<br>
<h2>Position Open</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="position_open">
<input type="hidden" name="position_open" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Position Close</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="position_close">
<input type="hidden" name="position_close" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Position Adjust</h2>
<p>Press this button to set the home position of your motor to zero</p>
<form action="position_adjust">
<input type="hidden" name="position_adjust" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Step 1 Calibration</h2>
<p>Press this button to automatically set the step count</p>
<form action="step_1">
<input type="hidden" name="step_1" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Step 3 Calibration</h2>
<p>Press this button to automatically calibrate the HIGH current settings</p>
<form action="step_3">
<input type="hidden" name="step_3" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Step 4 Calibration</h2>
<p>Press this button to automatically calibrate the LOW current setting</p>
<form action="step_4">
<input type="hidden" name="step_4" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Step 5 Calibration</h2>
<p>Press this button to automatically calibrate the LOW stall setting</p>
<form action="step_5">
<input type="hidden" name="step_5" value="1">
<input type="submit" value="Start">
</form>
<br>
<br>
<h2>Step 6 Calibration</h2>
<p>Press this button to automatically calibrate the LOW stall setting</p>
<form action="step_6">
<input type="hidden" name="step_6" value="1">
<input type="submit" value="Start">
</form>
*/
