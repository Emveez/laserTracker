#include "serialParser.h"


void serialParser::fetchSerial(){

	while(Serial.available() != 0){

		// Read one byte
		atmChar = Serial.read();

		if(atmChar == '\n') {

			this->parseCoordinates();
			//this->printer();
			atmString = "";

			break;

		} else {

			atmString += atmChar;

		} 

	}
}


void serialParser::parseCoordinates(){

	// Given a atmString we parse y and x
	char charIth;

	for (int i = 0; i < atmString.length(); ++i){

		charIth = atmString[i];

		//Serial.println(charIth);

		if (charIth == 'S') {
			squareCheck = charIth; // Put the check into this var
			charIth = '.';			
		}

		if ((charIth != '.')){

			//Serial.println(charIth);

			buffString += charIth;       

		} else {

			yCoordinate = buffString.toInt();
			buffString = "";

		}
	}

	xCoordinate = buffString.toInt();
	buffString = "";
}

void serialParser::setCoordinates(int &y, int &x, char &c){

	// Global variables
	y = yCoordinate;
	x = xCoordinate;
	c = squareCheck;

	// Reset to default
	squareCheck = '.';

}

void serialParser::printer(){

	Serial.print(yCoordinate);
	Serial.print('#');
	Serial.print(xCoordinate);
	Serial.print('\n');

}
