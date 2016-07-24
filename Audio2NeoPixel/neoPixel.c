/**
 * Encapsulate our interface to NeoPixels.
 *
 * @github https://github.com/nkolban/Audio2NeoPixel
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "neoPixel.h"
#include "i2c.h"
#define ARDUINO_ADDRESS (0x10)
#define MAX_AUDIO 1000000

static void HSVtoRGB( unsigned char *red, unsigned char *green, unsigned char *blue, float h, float s, float v );
static unsigned short numPixels;

void neopixel_init(unsigned short p_numPixels) {
  numPixels = p_numPixels;
  i2c_init(ARDUINO_ADDRESS);
}

/**
 * We are given a group of amplitudes for frequencies and we need to convert
 * this data into NeoPixel color and intensity values and then send them
 * via I2C to the Arduino to be displayed on the physical NeoPixels.
 */
void neopixel_process(double *data, int pixelCount) {
  char *rgbData = malloc(numPixels * 3);
  char *ptr = rgbData;
  int i;
  float red, green, blue;
  for (i=0; i<pixelCount; i++) {
    double value = data[i] / MAX_AUDIO;
    if (value > 1.0) {
      value = 1.0;
    }
    HSVtoRGB(ptr, ptr+1, ptr+2, 360*i/pixelCount, 1, value);
    ptr += 3;
  } // End of loop over each of the pixels.
  i=0;
  int j=pixelCount;
  while(j < numPixels) {
    *ptr = rgbData[i*3];
    ptr++;
    *ptr = rgbData[i*3+1];
    ptr++;
    *ptr = rgbData[i*3+2];
    ptr++;
    i++;
    if (i >= pixelCount) {
      i=0;
    }
    j++;
  }
  i2c_writePixels(rgbData, numPixels); // Send the pixels to the Arduino to the NeoPixel string.
  //printf("Writing data ...\n");
  free(rgbData);
} // End of neopixel_process


/**
 * Convert an HSV color value to an RGB color value.
 */
static void HSVtoRGB( unsigned char *red, unsigned char *green, unsigned char *blue, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	float r,g,b;
	if( s == 0 ) {
		// achromatic (grey)
		*red = *green = *blue = (unsigned char)(v*255);
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
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
  *red = (unsigned char)(r*255);
  *green = (unsigned char)(g*255);
  *blue = (unsigned char)(b*255);
} // End of HSVtoRGB
