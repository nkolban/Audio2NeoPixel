#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "neoPixel.h"
#include "i2c.h"
#define ARDUINO_ADDRESS (0x10)
#define MAX_AUDIO 1000000

static void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

void neopixel_init() {
  i2c_init(ARDUINO_ADDRESS);
}

/**
 * We are given a group of amplitudes for frequencies and we need to convert
 * this data into NeoPixel color and intensity values and then send them
 * via I2C to the Arduino to be displayed on the physical NeoPixels.
 */
void neopixel_process(double *data, int pixelCount) {
  char *rgbData = malloc(pixelCount * 3);
  char *ptr = rgbData;
  int i;
  float red, green, blue;
  for (i=0; i<pixelCount; i++) {
    double value = data[i] / MAX_AUDIO;
    if (value > 1.0) {
      value = 1.0;
    }
    HSVtoRGB(&red, &green, &blue, 360*i/pixelCount, 1, value);
    *ptr = (char)(red*255); ptr++;
    *ptr = (char)(green*255); ptr++;
    *ptr = (char)(blue*255); ptr++;    
  }
  i2c_writePixels(rgbData, pixelCount);
  printf("Writing data ...\n");
  free(rgbData);
}

static void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}
