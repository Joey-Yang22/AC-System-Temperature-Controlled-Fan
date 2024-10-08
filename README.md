# AC-System-Temperature-Controlled-Fan

Temperature Controlled Fan. System has two modes, MANUAL mode and AUTO mode. An LCD display will showcase what mode the system is currently in. Manual mode displays a percentage at what the fan's power is currently at.  Pushing the joystick up will increase the percentage by 10%. System cannot go below 0%, or higher than 100%. Auto mode displays the current temperature in the room using a thermistor (labeled Curr), and also a certain threshold temperature that the user can control (labeled Th). The DC motor (fan) will run depending on the difference between the current temperature and the threshold temperature. If the difference between the threshold and the current temp is 1, the percentage of the fan is running at 10%, difference of 2 makes the fan run at 20%. The pattern continues until the difference is greater than or equal to 10, where fan runs at 100%. If there is no difference and the threshold temperature is actually higher than the current temperature, the fan will run at 0%.


YOUTUBE LINK: https://youtu.be/RGgkL0LvwqI?si=AU5pvsDIHBlOsOhz&t=2
