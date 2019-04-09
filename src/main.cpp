#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <opencv2/highgui/highgui.hpp>
#include "usbcam.hpp"
#include "serialConnect.hpp"
#include "tools.hpp"

#define MICROPROCESSOR "/dev/ttyUSB0"
#define USBCAMERA "/dev/video0"
#define FRAME_W 640
#define FRAME_H 480
#define CAM_FPS	120

class trackProgram {

public:
	trackProgram();
	void menuProgram();
	void mainProgram();
	void calibratePotentiometer();
	void calibrateCrop();
	void squareFrame();
	void calibrateColor();
	void mapCoordinate2DAC();
	void trackBall();
	void setLaser(int y, int x, char c = '.');
	
private:

	// Cam handler
	USB_CAM *cam;
	cv::Mat atmFrame;

	// Serial handler
	serialHandler *serial;

	// Max voltage to DAC
	int maxDAC = 4090;
	
	// Mapped voltage from coordinate
	int xDAC;
	int yDAC;

	// Crop image coordinate
	cv::Rect cropRect;
	cv::Mat cropFrame;

	// HSV ranges, defualt i.e. tennis ball
	int H_l = 10, H_h = 64;
	int S_l = 96, S_h = 1453;
	int V_l = 0, V_h = 255;

	// Set the HSV threshold
	cv::Mat frame_HSV, frame_threshold, gaussFilter, outImg;

	// Ball position
	cv::Point ballPosition;

};


trackProgram::trackProgram(){

	cam = new USB_CAM(USBCAMERA, FRAME_W, FRAME_H, CAM_FPS);
	serial = new serialHandler(MICROPROCESSOR);

	cam->fetchFrame(atmFrame);

}


void trackProgram::setLaser(int y, int x, char c){

	// Perform check so x,y only in [0, maxDAC]
	if (!(y < 0 || y > maxDAC || x < 0 || x > maxDAC)) {

		serial->sendSerial(y, x, c);

	}

}


void trackProgram::calibratePotentiometer(){

	serial->sendSerial(0, 0, 'S'); // Send with S delimiter => calibrate mode
	cv::namedWindow("CAM", 1);

	// Approx 2 sec frame cap
	for (int i = 0; i < 600; ++i) {

		cam->fetchFrame(atmFrame);
		cv::imshow("CAM", atmFrame);
		cv::waitKey(1);

	}

	cv::destroyAllWindows();

}

void trackProgram::calibrateColor(){

	cv::namedWindow("HSV", 1);
	cv::namedWindow("CAM", 1);

	cv::createTrackbar("H_l", "HSV", &H_l, 180, NULL, NULL);
	cv::createTrackbar("H_h", "HSV", &H_h, 180, NULL, NULL);

	cv::createTrackbar("S_l", "HSV", &S_l, 255, NULL, NULL);
	cv::createTrackbar("S_h", "HSV", &S_h, 255, NULL, NULL);

	cv::createTrackbar("V_l", "HSV", &V_l, 255, NULL, NULL);
	cv::createTrackbar("V_h", "HSV", &V_h, 255, NULL, NULL);

	while(1) {

		cam->fetchFrame(atmFrame);

		cv::GaussianBlur(atmFrame, gaussFilter, cv::Size(9, 9), 0, 0 );
		cv::cvtColor(gaussFilter, frame_HSV, cv::COLOR_BGR2HSV);
		cv::inRange(frame_HSV, cv::Scalar(H_l, S_l, V_l), cv::Scalar(H_h, S_h, V_h), frame_threshold);

		cv::imshow("HSV", frame_threshold);
		cv::imshow("CAM", atmFrame);
		
		if (cv::waitKey(1) == 27) break;

	}

	cv::destroyAllWindows();

}

void trackProgram::calibrateCrop(){

	cv::namedWindow("Calibrate window", 1);

	cam->fetchFrame(atmFrame);		//For some reason we need to take a init frame
	serial->sendSerial(0, 0, 'S'); 	// Send with S delimiter => calibrate mode
	usleep(500000); // Sleep 0.5 sec
	cam->fetchFrame(atmFrame);
	cropRect = selectRect(atmFrame);

	cropFrame = atmFrame(cropRect);	

	cv::imshow("Calibrate window", cropFrame);
	cv::waitKey(0); 
	cv::destroyAllWindows();

}

void trackProgram::mapCoordinate2DAC(){

	// Dims of the cropped frame
	double width = cropFrame.cols; 
	double height = cropFrame.rows;

	double xDAC_frac = ballPosition.x/width;
	double yDAC_frac = ballPosition.y/height;

	// Reverse the coordinate
	xDAC = maxDAC - (int)maxDAC * xDAC_frac;
	yDAC = maxDAC - (int)maxDAC * yDAC_frac;

	// Send to mirror system
	serial->sendSerial(yDAC, xDAC, '.');

}

void trackProgram::trackBall(){

	cropFrame = atmFrame(cropRect);

	cv::GaussianBlur(cropFrame, gaussFilter, cv::Size(9, 9), 2, 2);
	cv::cvtColor(gaussFilter, frame_HSV, cv::COLOR_BGR2HSV);
	cv::inRange(frame_HSV, cv::Scalar(H_l, S_l, V_l), cv::Scalar(H_h, S_h, V_h), frame_threshold);

	cv::dilate(frame_threshold, outImg, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
	cv::erode(frame_threshold, outImg, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);

	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(outImg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    cv::Mat contourImage(outImg.size(), CV_8UC3, cv::Scalar(0,0,0));

	int largest_area = 0;
	int largest_contour_index = 0;
	cv::Rect bounding_rect;

    for (int idx = 0; idx < contours.size(); idx++) {

    	double area = cv::contourArea(contours[idx]); 

		if(area > largest_area) {
			largest_area = area;
			largest_contour_index = idx; //Store the index of largest contour
			bounding_rect = cv::boundingRect(contours[idx]); // Find the bounding rectangle for biggest contour
		}
    
    }

    ballPosition = (bounding_rect.br() + bounding_rect.tl())*0.5; // Middle of rectangle
    cv::rectangle(cropFrame, bounding_rect, cv::Scalar(0, 255, 0));
	cv::circle(cropFrame, ballPosition, 3, cv::Scalar(0,0,255));

}

void trackProgram::mainProgram(){

    int fpsInt, count = 0;
    time_t start = time(0);
    double timeDiff;

    cv::namedWindow("HSV", 1);
    cv::namedWindow("CAM", 1);
	while(1){

		cam->fetchFrame(atmFrame);

		// Trackball
		this->trackBall();

		cv::imshow("HSV", frame_threshold);
		cv::imshow("CAM", cropFrame); 

		// Map to DAC units and send to mirrors
		this->mapCoordinate2DAC();

		if(cv::waitKey(1) >= 0) break;

		// FPS stuff
		count++;
		timeDiff = difftime(time(0), start);
		if ((int)timeDiff != 0){
	        fpsInt = (int)count/timeDiff;;
		}

		std::cout << "\r" << "FPS: " << fpsInt << std::endl;

	}

	std::cout  << "\n" << std::endl;
	cv::destroyAllWindows();
}

void trackProgram::menuProgram(){

	char selection;
	bool checker = false;
	bool frameCrop = false;

	while(1) {

		for (int i = 0; i < 100; ++i){
			std::cout<<"\n";
		}

		std::cout<<"\nMenu";
		std::cout<<"\n====================";
		std::cout<<"\n1 - Calibrate potentiometer ";
		std::cout<<"\n2 - Calibrate colors ";
		std::cout<<"\n3 - Calibrate crop of frame ";
		std::cout<<"\n4 - Program ";
		std::cout<<"\n5 - Exit";
		std::cout<<"\n====================";
		std::cout<<"\nEnter selection: ";

		std::cin>>selection;

		switch(selection) {

		case '1':{
			this->calibratePotentiometer();
		}
			break;

		case '2':{

			std::cout<<"\n Press ESC to quit";
			this->calibrateColor();

		}
			break;

		case '3':{
			this->calibrateCrop();
			frameCrop = true;
		}
			break;

		case '4':{
			if (frameCrop) {
				this->mainProgram();
			} else {
				std::cout << "Crop frame!" << std::endl;
				usleep(1000000);
			}
		}
			break;

		case '5':{
			checker = true;
		}
			break;

		default:
			std::cout<<"\n Invalid selection";

		}

		std::cout<<"\n";

		if (checker) break;

	}

}


int main(){

	trackProgram p;

	p.menuProgram();

	return 0;
}