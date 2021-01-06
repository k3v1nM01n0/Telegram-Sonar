# Description

Water level sensor using a HC-SR04 ultrasonic sensor to detect level of water in a tank.
The user receives notifications and alerts via a telegram bot customized depending on the current level of water.
The HC-SR04 ultrasonic sensor uses sonar to determine distance to the water level.
For this project an Arduino library [newPing](./https://playground.arduino.cc/Code/NewPing/) is used.
The sensor is wired to an esp8266 microcontroller.

     

# Features

    >Read level of water remmaining in the tank
    >Computes the level remmaining
    >Send the data as a notification to a telegram bot ever one hour

## Note ##
 +HC-SR04+ (+) sign sensor is works with 5V 
 +Not  suitable for direct water level monitor.(minnimum of 10 cm from water surface
 +Sensor accuracy depend on the quality of the sensor 
