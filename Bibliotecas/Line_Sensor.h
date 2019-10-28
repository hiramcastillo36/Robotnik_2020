/*************************************************************************/
/*                                                                       */
/*                                                                       */
/****************************CHABOTS ROBOTNIK*****************************/
/*                                                                       */
/*Line Sensor V1.0														 */	
/*S.L.P MEXICO                           TIME:10:37:40 PM DATE:10/27/2019*/
/*************************************************************************/

#ifndef Line_Sensor_h
#define Line_Sensor_h

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

class Line_Sensor{
	public:
		Line_Sensor(byte port[], int umbral, byte pinNeopixel);
		bool isLine();
		void setColor(byte r, byte g, byte b);
		void itsMomentToFun(byte r, byte g, byte b);
	private:
		int _umbral;
		byte _port[];
		byte _r;
		byte _g;
		byre _b;
		byte _pinNeopixel;
		const byte numPixels = 3;
};


#endif