/**
 * Encapsulate I2C communication to the Arduino.
 *
 * @github https://github.com/nkolban/Audio2NeoPixel
 */
#include <wiringPiI2C.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "i2c.h"

/**
 * The file descriptor representing our pipe through I2C to the Arduino.
 */
static int fd;


/**
 * Initialize the I2C environment.
 */
void i2c_init(int deviceAddress) {
  wiringPiSetupGpio();
  fd = wiringPiI2CSetup(deviceAddress);
  if (fd == -1) {
    printf("Error starting I2C!");
    return;
  }
} // End of i2c_init


/**
 * Send a stream of pixels that are found in the buffer down the I2C channel.
 * The data stream is composed of 2 byte length, followed by 3 byte pixels that
 * repeat for the length sequence.  Each pixel is composed of 3 bytes, one for
 * each of the red, green and blue channels.
 */
void i2c_writePixels(char *buffer, int pixels) {
  int result;
  int i;
  unsigned short pixelsNetworkByteOrder = htons(pixels);      // Convert the size to network byte order
  write(fd, &pixelsNetworkByteOrder, sizeof(pixelsNetworkByteOrder)); // Send the count of pixels down the I2C bus
  //usleep(20*1000);
  for (i=0; i<pixels; i++) {               // Loop over each of the pixels (3 bytes in size)
    result = write(fd, buffer, 3);         // Send the next pixel down the I2C bus
    //usleep(20*1000);
    buffer+=3;                             // Each pixel is 3 bytes long, skip the 3 bytes just sent.
    if (result == -1) {
        perror("Error writing to I2C");
    }
  } // End of loop over each of the pixels.
} // End of i2c_writePixels
