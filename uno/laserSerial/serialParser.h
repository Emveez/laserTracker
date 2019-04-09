#ifndef serialParserH
#define serialParserH

#include <Arduino.h>

class serialParser {

private:
	char atmChar;
	char squareCheck;
	String buffString;
	String atmString = "";

	int yCoordinate;
	int xCoordinate;

public:

	void fetchSerial();
	void parseCoordinates();
	void setCoordinates(int &y, int &x, char &c);
	void printer();

};



#endif