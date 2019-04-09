#include "serialConnect.hpp"
#include <iostream>

serialHandler::serialHandler(){

}

serialHandler::serialHandler(const char *m){

	serial_port = open(m, O_RDWR);

	memset(&tty, 0, sizeof tty);

	// Read in existing settings, and handle any error
	if(tcgetattr(serial_port, &tty) != 0) {
	    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~CRTSCTS; 
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;
	tty.c_lflag &= ~ECHOE;
	tty.c_lflag &= ~ECHONL;
	tty.c_lflag &= ~ISIG;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

	tty.c_oflag &= ~OPOST;
	tty.c_oflag &= ~ONLCR;

	// Baud rate
	cfsetispeed(&tty, BAUDRATE);
	cfsetospeed(&tty, BAUDRATE);

	// Save tty settings, also checking for error
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
	    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	}

}

void serialHandler::sendSerial(int y, int x, char c){

	int lengthY = snprintf(NULL, 0, "%d", y);
	int lengthX = snprintf(NULL, 0, "%d", x);
	int totalLen = lengthY + lengthX + 2;

	// Allocate memory for string
	char* str = (char*)malloc(totalLen);

	sprintf(str, "%d%c%d\n", y, c, x);
	//sprintf(str, "%d.%d\n", y, x);

	//std::cout << str << std::endl;

	//printf("%s\n", str);

	write(serial_port, str, totalLen);

	// Free mem
	free(str);

}

serialHandler::~serialHandler(){

	close(serial_port);

}
