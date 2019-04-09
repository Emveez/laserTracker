#include "DAC_MCP4X.h"
#include "serialParser.h"

// Handling the two laser axis
MCP4X dac;

// Parse info from serial. Set the global coordinate y, x
serialParser parser;

char check = '.';
int y = 1, x = 1;
int maxDAC = 4090;

void setup() {

	Serial.begin(115200);  
	Serial.println("Start Serial Monitor");

  // DAC init 
	dac.init(MCP4X_4822, 5000, 5000,
	10, 7, 1);
	dac.setGain2x(MCP4X_CHAN_A, 0);
	dac.setGain2x(MCP4X_CHAN_B, 0);
	dac.begin(1);

}

// set laser to y, x coordinates
// The coordinate should be in volt measure i.e. max = 4090
void setLaser(){

    dac.output2(y, x);

}

// Function that make a square for calibaration
void calibrate() {
	
	Serial.println("Calibrate mode");
	for (int i = 0; i < 3000; ++i){

		dac.output2(0, 0);
		delay(1);
		dac.output2(0, 4090);
		delay(1);
		dac.output2(4090, 4090);
		delay(1);
		dac.output2(4090, 0);
		delay(1);

	}
 
}


void loop() {


	parser.fetchSerial();
  parser.printer();
	parser.setCoordinates(y, x, check);

	if (check == 'S') {
		calibrate();
	}

	setLaser();

}
