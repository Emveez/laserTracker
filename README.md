# Laser projecting system
Real time object tracking system that projects a laser beam on object using laser galvanometers and a camera as sensor. The laser galvanometer system is built similar to [this](https://www.instructables.com/id/Arduino-Laser-Show-With-Real-Galvos/). An Arduino nano is used to regulate the galvanometers and the data transmission is done over serial communication. The tracking object is a regular tennis ball and to track the object basic computer vision techniques are used. The process for tracking the ball is the following
* Remove noise using a Gaussian blur
* Convert RGB to HSV and isolate ball color
* Dilate/Erode to remove noise
* Find contours with largest area
* Draw bounding box and find middle coordinate
# 
With this setup tracking can be done around 50 fps which can be seen in the following GIF, the red dot is the middle of the box
![Alt Text](https://media.giphy.com/media/kE34Vx0m7oQGILEsQp/giphy.gif)

# Demo
Here is a demo when the laser is projected to the object
![Alt Text](https://media.giphy.com/media/dZWVwc7VbaYacBtd18/giphy.gif)
