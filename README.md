# Webcam Object Measurement - OpenCV

The aim of this project is to develop an algorithm that detects an object in the image and converts the detected object’s size in pixels into a given unit of measurement. In this project a webcam is used to detect objects and measure the dimensions of the detected objects in millimetres. This project uses OpenCV edge detection approach. I detected a triangle and used it as a reference measure for the rest of the image.

This project uses 2D diemension so it does not consider the depth. Therefore a known distance must be set between the camera and the desired objects to measure. In my case I used 500mm distance. I hope you find this usefull. 