---
layout: page
title: Tuning Motors Guide
permalink: /VALAR_W1/tuning_motors
parent: Tulkas
nav_order: 5
---

<p>Your MorningRod uses a very advanced stepper motor and driver to move the motor. </p>
<p>Instead of using a sensor to detect when the curtain has reached either end, it counts the number of steps to move in each direciton based on the length (in inches) of the rods. In addition, it can also detect when it reaches eatierh end by stalling. The motor driver measures the back EMF of the motor. When the curtains stall and cannot move, the back EMF sharply increases and the driver knows to shut it off.</p>
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

