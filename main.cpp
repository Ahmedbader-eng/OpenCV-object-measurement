
#include <opencv2\opencv.hpp>


// Initialising the threshold range to detect edges using canny
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3; // canny recomends upper.lower ratio of 3:1


int main(int argc, char* argv[])
{
	//initialise camera object in VideoCapture calss to display the webcam view 
	// the webcam index is 0
	cv::VideoCapture camera(0);

	// if the camera is not accessible the program will terminate 
	if (!camera.isOpened()) {
		std::cout << "Could not access the camera" << std::endl;
		return -1;
	}

	while (true) {

		// frame is a mat object to display every frame captured by camera to display a video
		cv::Mat frame;
		//copy the captured image in camera object to frame object
		camera >> frame;

		// canny variables to store the detected edges
		cv::Mat cannyEdges, dst;
		cv::Mat src; // src is a Mat object to hold the image in gray scale for canny detection 
		src = frame.clone(); // deep copy of the captured frame by the camera to the src object
		

		//dst object must have the same size and type of the frame to take all the edges detected
		dst.create(frame.size(), frame.type());

		// a gray scale image is good for detection therefore the src image is converted to gray scale
		cv::cvtColor(src, src, CV_BGR2GRAY);

		// blure to reduce the noise in the image, here te blure uses kernal size of 3 x 3
		cv::blur(src, cannyEdges, cv::Size(3, 3));

		// Create a Trackbar for the user to enter the lower threshold for the Canny detector
		// "Min Threshold:" the trackbar name 
		// "Edges" the parent window for the trackbar
		// on the creation of the trackbar the slider will be positioned at the lowThreshold value
		// lowThreshold value will change depending on the position of the slider
		// the NULL pointer is to tell the trackbar to only update the value
		cv::createTrackbar("Min Threshold:", "Edges", &lowThreshold, max_lowThreshold, NULL);

		// the input and output images are the same 
		// the the threshold is to limit the range of pixel gradient
		// if a pixel gradient is lower than the lower threshold it is rejected
		// if a pixel gradient is higher than the upper threshold it is accepted 
		// pixels between the threshold limit are only accepted if they are connected to 
		//a pixel that is higher than the upper threshold
		cv::Canny(cannyEdges, cannyEdges, lowThreshold, lowThreshold*ratio); 

		// create a window to display the edges
		// the window name is edges with the flag CV_WINDOW_AUTOSIZE to display the image with its original size
		// this is limited to the screan resolution so the window may scale the image to fit
		cv::namedWindow("Edges", CV_WINDOW_AUTOSIZE);

		// set all the elements in dst matrix to 0 to create a black image 
		dst = cv::Scalar::all(0);

		// coppy the frame object onto dst but only copy the copy the pixels in the locations where they have non-zero values. 
		//So the detected edges are copied to the black image in dst.
		frame.copyTo(dst, cannyEdges);
		cv::imshow("Edges", dst);	// display the detected edges in a black background 

		/// Contours are detected in the thresholded image.
		// a vector of vector of points to store the contours i.e. the edges detected 
		std::vector<std::vector<cv::Point> > contours; 
		std::vector<cv::Vec4i> hierarchy; // it will contain information about the image topology.


		// findCounours is used to find detect the edes in the input image
		// CannyEdges is the input image 
		// contours is the detected contours i.e. edges. Each contour is stored as a vector of points.
		// findContours outputs information about the image topology and store it in hierarchy
		/* CV_RETR_TREE is the flag for the contour retrieval mode which retrieves all of the 
		contours and reconstructs a full hierarchy of nested contours*/
		/* CV_CHAIN_APPROX_SIMPLE is the flag for Contour approximation method which compresses 
		horizontal, vertical, and diagonal segments and leaves only their end points*/
		cv::findContours(cannyEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

		cv::Mat drawing = frame.clone();	// deep copy so drawing object has its own pixels
		float fpixTo_mm_global; // to store the conversion value

		//loop through the contours to detect objects
		for (int i = 0; i < contours.size(); i++) {
			// to only detect the rectangle in the given image. 
			// the area of the contours must include the rectangle only
			// the rectangle area was found to be between (11000 - 13000) pixels^s 
			// when the object is 490+-20 mm away from the camera
			if ((cv::contourArea(contours[i]) < 13000) && ((cv::contourArea(contours[i]) > 11000))) {

				// draw counters which is the edges detected
				// drawing is the input image to draw contours on it
				// contours is the edges to be drawn
				// i is the index to indicat which contour to draw
				// contours can be drawn in the desired colour here they are drawn in blue
				//the thickness of the line that draws the contour is set to 3 pixels
				cv::drawContours(drawing, contours, i, cv::Scalar(255, 0, 0), 3);

				// initialising a box object in Rect class to access a rectangle information ie height and width
				// boundingRect function does not consider rotation there fore it draws a stright rectangle
				// rotating the object will not minimuse the drwn rectangle to fit the object
				cv::Rect box = cv::boundingRect(contours[i]);

				// rectangle function simply draws a rectangle on the specified image
				// drawing is the image to draw a rectangle on
				// box object has the vertices of the rectangle need to be drawn
				// Scalar() is used to set the color of the rectangle
				// the thickness of the rectangle lines is set to 3 pixels
				cv::rectangle(drawing, box, cv::Scalar(0, 255, 0), 3);	
																					
				// the conversion is set by practical measurement. 
				//This conversion is true for a 500 mm distance between the camera and the object 2D plane
				// The object plane must be parallel to the camera view plane. 
				// 168 mm of rectangle width measured practically from a printed black rectangle shape
				// 216 pixels of rectangle width was measured before converting pixels to mm 
				// the object was set at distance of 500mm away from the camera for both measurements
				float fpixTo_mm = 169.0 / box.width;
				fpixTo_mm_global = fpixTo_mm;

				// print the the dimensions of the drawn recangle
				std::cout << "Rectangle Width = " << box.width*fpixTo_mm_global << " mm, " << "Height = " << box.height*fpixTo_mm_global << " mm" << std::endl;
				

			}

			// any object with area bigger than 13000 pixels^s will be detected, the trackbar can be used to adjust the threshold
			// all objects are detected in the same way as the rectangle
			// in this project the detected image has only a rectangle and a circle
			if (cv::contourArea(contours[i]) > 13000){

				cv::drawContours(drawing, contours, i, cv::Scalar(255, 0, 0), 3);

				cv::Rect box = cv::boundingRect(contours[i]);

				cv::rectangle(drawing, box, cv::Scalar(0, 255, 0), 3);

				// the sidth or height of the drawn rectangle is basically the circle diameter 
				std::cout << "Circle Diameter = " << box.height*fpixTo_mm_global << std::endl;

			}
		}

		// display the image with the object detection 
		cv::imshow("Contours", drawing);

		// waitKey waits for the user input for a number of milliseonds 
		char c = cv::waitKey(1);
		if (c == 'q') { // if the user press enters q break from the while loop
			break;
		}
	}

	// close all of the opened HighGUI windows.
	cv::destroyAllWindows();

	return 0;
}