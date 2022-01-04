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
const char WIFI_HTML[] = "Enter your home Wifi Name and Password <br> <br> <form action=\"/set_wifi\">\n    <label class=\"label\">Network Name</label>\n    <input type = \"text\" name = \"ssid\"/>\n    <br/>\n    <label>Network Password</label>\n    <input type = \"text\" name = \"pass\"/>\n    <br/>\n    <input type=\"submit\" value=\"Set Values\">\n</form>";

/*
Enter the following text into https://tomeko.net/online_tools/cpp_text_escape.php?lang=en

<h2>Valar Systems</h2>
<h3>Motion Control</h3>
<p>To learn more, please visit <a href=\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\">https://help.valarsystems.com</a></p>
<p>To add this device to your network <a href=\"http://192.168.4.1/wifi\">go to http://192.168.4.1/wifi</a></p>
<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>
<br>
<h2>Position</h2>
<form action="/position">    
<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>
<label><b>move_to :</b></label>    
<input value = %PLACEHOLDER_PERCENT% type = "text" name = "move_percent">
<br/>
<input type="submit" value="Set Position">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_percent=%PLACEHOLDER_PERCENT%</p>
</form>
<br>
<h2>Motor Parameters</h2>
<form action="/set_motor">
<h3>Speed Setting</h3>
<p>Enter a value from 10-200. This is the amount of RMS current the motor will draw.</p>
<label><b>speed: </b></label>
<input value = "%PLACEHOLDER_MAX_SPEED%" type = "text" name = "max_speed">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_speed=%PLACEHOLDER_MAX_SPEED%</p>
<br>
<h3>Current Setting</h3>
<p>Enter a value from 400-2000. This is the amount of RMS current the motor will draw.</p>
<label><b>current: </b></label>
<input value = "%PLACEHOLDER_CURRENT%" type = "text" name = "current">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?current=%PLACEHOLDER_MAX_STEPS%</p>
<br>
<h3>Stall Setting</h3>
<p>Enter a stall value from 0-255. The higher the value, the easier it will stall.</p>
<label><b>stall: </b></label>
<input value = "%PLACEHOLDER_STALL%" type = "text" name = "stall">
<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall=%PLACEHOLDER_STALL%</p>
<br>
<input type="submit" value="Set Parameters">
</form>
<br>
<br>
<p>Press this button to set the home position of your motor to zero</p>
<form action="set_zero">
<input type="hidden" name="set_zero" value="1">
<input type="submit" value="set_zero">
</form>
*/

const char SETTINGS_HTML[] = "<h2>Valar Systems</h2>\n<h3>Motion Control</h3>\n<p>To learn more, please visit <a href=\\\"https://help.valarsystems.com/docs/VAL-1000/VAL-1000\\\">https://help.valarsystems.com</a></p>\n<p>To add this device to your network <a href=\\\"http://192.168.4.1/wifi\\\">go to http://192.168.4.1/wifi</a></p>\n<p>To remove this device from your network, press and hold the wifi reset button for 3+ seconds.</p>\n<br>\n<h2>Position</h2>\n<form action=\"/position\">    \n<p>Enter a value from 0-100. This is the percent of the max_steps value to move the motor.</p>\n<label><b>move_to :</b></label>    \n<input value = %PLACEHOLDER_PERCENT% type = \"text\" name = \"move_percent\">\n<br/>\n<input type=\"submit\" value=\"Set Position\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/position?move_percent=%PLACEHOLDER_PERCENT%</p>\n</form>\n<br>\n<h2>Motor Parameters</h2>\n<form action=\"/set_motor\">\n<h3>Speed Setting</h3>\n<p>Enter a value from 10-200. This is the amount of RMS current the motor will draw.</p>\n<label><b>speed: </b></label>\n<input value = \"%PLACEHOLDER_MAX_SPEED%\" type = \"text\" name = \"max_speed\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?max_speed=%PLACEHOLDER_MAX_SPEED%</p>\n<br>\n<h3>Current Setting</h3>\n<p>Enter a value from 400-2000. This is the amount of RMS current the motor will draw.</p>\n<label><b>current: </b></label>\n<input value = \"%PLACEHOLDER_CURRENT%\" type = \"text\" name = \"current\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?current=%PLACEHOLDER_MAX_STEPS%</p>\n<br>\n<h3>Stall Setting</h3>\n<p>Enter a stall value from 0-255. The higher the value, the easier it will stall.</p>\n<label><b>stall: </b></label>\n<input value = \"%PLACEHOLDER_STALL%\" type = \"text\" name = \"stall\">\n<p>You can also send an HTTP request to http://%PLACEHOLDER_IP_ADDRESS%/set_motor?stall=%PLACEHOLDER_STALL%</p>\n<br>\n<input type=\"submit\" value=\"Set Parameters\">\n</form>\n<br>\n<br>\n<p>Press this button to set the home position of your motor to zero</p>\n<form action=\"set_zero\">\n<input type=\"hidden\" name=\"set_zero\" value=\"1\">\n<input type=\"submit\" value=\"set_zero\">\n</form>";
