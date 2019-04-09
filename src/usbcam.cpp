#include "usbcam.hpp"


USB_CAM::USB_CAM(){
	// Default constructor
}


USB_CAM::USB_CAM(const char *v, int w, int h, int fps){


	frameW = w;
	frameH = h;

    if((dev = open(v, O_RDWR)) < 0){
        perror("Problem file video0 missing i.e. connect camera or select right index");
        exit(1);
    }

	struct v4l2_capability cap;
	memset(&cap, 0, sizeof cap);
	if(ioctl(dev, VIDIOC_QUERYCAP, &cap) < 0){
	    perror("VIDIOC_QUERYCAP");
	    exit(1);
	}


	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = w;
	fmt.fmt.pix.height = h;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_ANY;

	if(ioctl(dev, VIDIOC_S_FMT, &fmt) < 0){
	    perror("VIDIOC_QUERYCAP");
	    exit(1);
	}
		

	struct v4l2_streamparm parm;
	memset(&parm, 0, sizeof parm);
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if(ioctl(dev, VIDIOC_G_PARM, &parm) < 0){
	    perror("VIDIOC_G_PARM 1");
	    exit(1);
	}	

	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = fps;
	if(ioctl(dev, VIDIOC_S_PARM, &parm) < 0){
	    perror("VIDIOC_G_PARM 2");
	    exit(1);
	}	

	struct v4l2_requestbuffers rb;
	memset(&rb, 0, sizeof rb);
	rb.count = 1;
	rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	rb.memory = V4L2_MEMORY_MMAP;

	if(ioctl(dev, VIDIOC_REQBUFS, &rb) < 0){
	    perror("VIDIOC_REQBUFS");
	    exit(1);
	}	



	memset(&buf0, 0, sizeof buf0);
	buf0.index = 0;
	buf0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf0.memory = V4L2_MEMORY_MMAP;

	if(ioctl(dev, VIDIOC_QUERYBUF, &buf0) < 0){
	    perror("VIDIOC_QUERYBUF");
	    exit(1);
	}	

	// Map data to memory
	mem0 = mmap(0, buf0.length, PROT_READ, MAP_SHARED, dev, buf0.m.offset);

	memset(&buf0, 0, sizeof buf0);
	buf0.index = 0;
	buf0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf0.memory = V4L2_MEMORY_MMAP;

	if(ioctl(dev, VIDIOC_QBUF, &buf0) < 0){
	    perror("VIDIOC_QBUF");
	    exit(1);
	}	

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(dev, VIDIOC_STREAMON, &type) < 0){
	    perror("VIDIOC_STREAMON");
	    exit(1);
	}	

	printf("USB CAMERA INIT OK \n");
}


USB_CAM::~USB_CAM(){

	// Close streaming
	if(ioctl(dev, VIDIOC_STREAMOFF, &type) < 0){
	    perror("VIDIOC_STREAMOFF");
	    exit(1);
	}

	std::cout << "Ending USB CAM" << std::endl;

}


void USB_CAM::fetchFrame(cv::Mat &frame){

	memset(&buf0, 0, sizeof buf0);
	buf0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf0.memory = V4L2_MEMORY_MMAP;
	ioctl(dev, VIDIOC_DQBUF, &buf0);

	cv::Mat atmFrame = cv::Mat(frameH, frameW, CV_8UC3, (int*)mem0);

    frame = cv::imdecode(atmFrame, 1);
	
	if(ioctl(dev, VIDIOC_QBUF, &buf0) < 0){
	    perror("VIDIOC_STREAMOFF");
	    exit(1);
	}

}