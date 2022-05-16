# MazeGame

Links to Parts:
Adafruit LIS3DH Triple-Axis Accelerometer (+-2g/4g/8g/16g) : ID 2809 : $4.95 : Adafruit Industries, Unique & fun DIY electronics and kits
Mini Metal Speaker w/ Wires - 8 ohm 0.5W : ID 1890 : $1.95 : Adafruit Industries, Unique & fun DIY electronics and kits

What kinds of internal peripherals will you use? (at least four)
SPI (Screen), I2C (Accelerometer), Timer (Game time), DAC (Game music), GPIO (Buttons)

What kinds of external interface formats will you use? (at least two)
SPI, I2C, GPIO, Analog

Briefly, what are you making and what are you making it out of?
We are going to make a handheld maze game where you control the ball’s movement by rotating the device relative to gravity. The game will be displayed on the MSP2202 TFT LCD Display and angular position will be provided by an LIS3DH triple-axis accelerometer. Tetris music will be played with the MIDI interpreter used in lab via a M2850-8B-0L03R speaker powered by an LM386 audio amplifier. The LM386 is designed for low voltage consumer applications and is adequately powered by a 5 volt supply. The 24AA32AF EEPROM will save the high score even after the device is shut down and restarted. Five tactile push buttons (up/down/left/right/select) will allow the user to navigate any necessary prompts and menus.

Mazes will be dynamically generated. As mazes are completed, they will increase in size and number of rotations required to solve them. A reasonable minimum/maximum rotation limit will be put in place for each size of maze generated.

Briefly, how will you demonstrate it?
We will have the TA play the game. Each maze must be completed within a time limit or the speaker will buzz. The score achieved and the all-time high score will be displayed. The player will be given the choice to start a new game.

Briefly, how will we define the criteria for a successful project? (If your demonstration does not meet these criteria, your score will be negatively affected.)
The rotation based control should track the device’s position accurately and be free of noise/glitches. The game should be able to be played and completed by any adult with no instructions given (any instructions should be easily accessible within the device itself). The maze levels should gradually increase in size as each subsequent maze is completed. The maze size will level off at a point before the maze paths become hard to discern (however we expect people to lose or quit before this point is reached). The player’s score should be a summation of the time left on the countdown for each maze successfully completed. For example, if a certain level has a time limit of 45 seconds, and the player completes the maze in 30 seconds, 15 points will be added to the player’s total score. The high score should be saved and able to be recalled even after the device is shutdown and powered back on. Music should be clearly audible and free of noise.



#CONCLUSION

What did you build? 	
What components (including model numbers) did you use? 	
What challenges did you face, and how did you overcome them? 	
What do you recommend to anyone doing the same thing or using the same components in the future?

We built a maze game that was controlled by rotation of the board. The maze was dynamically generated and increased in size when each level was passed. Players had a set amount of time, which increased as the game continued, to solve each level. Time remaining and current score were displayed on the maze screen. Level score was equal to the amount of time remaining to complete the maze. Menus at start, next level, and end of game were navigated using push buttons and GPIO. After the level or game ended, score was displayed and the player could choose to continue/quit or start a new game. Navigation within the maze was controlled by an accelerometer that interfaced with I2C. EEPROM was used with I2C to save the high score when the game was reset or power was turned off and on. Tetris music played during gameplay and was set up with a MIDI interpreter and DAC output.
The game was displayed on the MSP2202 TFT LCD Display and angular position was provided by an LIS3DH triple-axis accelerometer. 24AA32AF EEPROM allowed our team to save the high score after power cycles. An LM386 amplifier drove a small 0.5 watt M2850-8B-0L03R speaker. Generic 6mm buttons provided the user-input, and LM1117 regulators allowed the team to power the components at their respective voltages.
One major issue that we encountered was related to the randomness of the maze that we were generating. When we initially wrote the maze program we used time.h to seed the random number generator. However, we learned that time.h cannot be used in our microcontroller. After this, we followed the directions on the references site about seeding random with a timer. Our first attempt at this failed and the mazes were still not random. We realized that this was due to the fact that we were calling srand(TIM2->CNT) at the same time each time the game started. In order to fix this we waited for a button to be pressed before we seeded srand, to ensure a different seed each time. 

We also struggled to correctly display colors. We used GIMP to generate the ball, and its output was not displaying colors correctly. We got around this by copying the number of the color we wanted into the array manually. However, the color was lime green against a white background, and the lack of contrast made the ball difficult to view if the player held the maze screen far away.

One final issue we encountered involved music. We used a weak 0.5 W speaker connected to a LM386 amplifier, and getting output at a reasonable volume was difficult. We needed overly complex circuitry to get volume where you didn’t need to place your ear near the speaker to hear. We also struggled with enabling and disabling the music at correct times in the code. When music was stopped and restarted, there was a remaining contribution that interfered with the sound. To fix this, we disabled the timer and set the voices array values to zero before restarting the music. We also did not correctly set the interrupt priority for the music, and the sound clicked as a result. We did not realize this because of the low output of the speaker, but it became clear during the demonstration when we hooked up the output to the lab speakers. If we had tested with the lab speaker, we would likely have been able to fix this issue.

I would recommend a similar project to teams in the future. We ran into many challenges arriving at our final design, however all problems were manageable with proper time management and teamwork. Every component worked well with the exception of the LM386 audio amplifier. We found the LM386 to be very temperamental and prone to noise on our breadboard, and the suggested schematics in the datasheet had to be modified to attain usable results in our case. A small issue we had with the LIS3DH accelerometer was that the datasheet I2C section indicated that a repeated start bit must be sent when the master is receiving data from the accelerometer. We couldn’t figure out how to do this, but after research we found that the repeated start wasn’t necessary- sending a stop bit first followed by a start bit also functioned properly. Once this was figured out the accelerometer worked flawlessly.
