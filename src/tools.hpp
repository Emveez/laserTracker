#include <opencv2/opencv.hpp>


struct SelectionState {
	cv::Point startPt, endPt, mousePos;
	bool started = false, done = false;

	cv::Rect toRect();
};

void onMouse(int event, int x, int y, int, void *data);

cv::Rect selectRect(cv::Mat image, cv::Scalar color = cv::Scalar(255,0,0), int thickness=2);