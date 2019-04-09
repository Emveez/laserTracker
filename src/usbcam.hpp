#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <iostream>

/* 
	Handling all the parsing of a frame from the /dev/video0 file using v4l2
	and stored in a opencv matrix type frame. 
	Most of the camera setup comes from the USB CAM SDK

*/

class USB_CAM {

public:
	USB_CAM();
	USB_CAM(const char *v, int w, int h, int fps);
	~USB_CAM();

	void fetchFrame(cv::Mat &frame);

private:

	void *mem0;
	int dev;
	int type; // Store type of video buff saver
	struct v4l2_buffer buf0;

	int frameW;
	int frameH;

};