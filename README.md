# Washing Machine Control Panel Project

In this project, I will be programming the Nucleo-64 STM32L476 Microcontroller using C++ :

![image](https://user-images.githubusercontent.com/121032332/208891648-20073574-26a8-4a57-92a6-4338720a5b55.png)

# Learning Outcome
Created simulation of a washing machine control panel by making use of electrical components such as potentiometers, temperature sensors, light dependent resistors, force-sensitive resistors, buttons, 7 - segment display, single-colored LEDs, and multi-colored LEDs.


Circuit layout :

![image](https://user-images.githubusercontent.com/121032332/208889935-4d9e4bb5-cade-4db6-99cc-59cb10fcd261.png)


Block Diagram :

![Screenshot (88)](https://user-images.githubusercontent.com/121032332/208940023-c1e52af8-d098-4d22-879b-37bb7cc89ada.png)


# Demonstration video :

Pressed "power" button and green led lit up indicating the control panel is turned on :

https://user-images.githubusercontent.com/121032332/208926628-407dc539-fbb1-48a9-953c-f1acf4d6685f.mp4


Force sensitive resistor acts as a door sensor to detect if the door is secured :

Buzzer will sound to notify the user that the door is not secured properly and the multi-colored LED will remained red.

https://user-images.githubusercontent.com/121032332/208927248-d7097b12-5d5f-45dc-b328-6335d716995c.mp4

The multi-colored LED illuminating from red to green indicates the door sensor is secured.

Rotate the potentiometer to select between two modes and when ready, pressed "Start/Pause" button to start the washing cycle process :

The 7 - segment component will display "CYCLE BEGIN" and the timer will start counting down for each stages, washing, rinsing and spinning.

https://user-images.githubusercontent.com/121032332/208931186-57aff22f-b6b8-4955-b8a0-fd6102deeeb9.mp4


Pressed "Restart" button to restart the washing cycle without changing the mode selections :

https://user-images.githubusercontent.com/121032332/208931925-80cc12ef-c6c7-40cf-8956-cdfb375b988d.mp4


Pressed "Start/Pause" button to pause during wash cycle and pressed again to continue the wash cycle :

https://user-images.githubusercontent.com/121032332/208938103-4ffddc57-2e15-462d-a931-ef15d336034c.mp4


Pressed "Power" button again to turn off the system :

https://user-images.githubusercontent.com/121032332/208934175-23a1cd1f-8894-4aed-8f24-ba0bf936e679.mp4

