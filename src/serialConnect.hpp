#include <stdio.h>
#include <string.h>
#include <string>
#include <fcntl.h> 
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <stdlib.h>


#define BAUDRATE B115200

/* 
	Serial communication using the termios library 

*/

class serialHandler {

public:
	serialHandler();
	serialHandler(const char *m);
	void sendSerial(int y, int x, char c);
	~serialHandler();

private:

	struct termios tty;
	int serial_port;
	
};

