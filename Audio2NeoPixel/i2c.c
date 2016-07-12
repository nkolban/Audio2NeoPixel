#include <wiringPiI2C.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "i2c.h"

static int fd;

void i2c_init(int deviceAddress) {
  wiringPiSetupGpio();
  fd = wiringPiI2CSetup(deviceAddress);
  if (fd == -1) {
    printf("Error starting I2C!");
    return;
  }
}

void i2c_writePixels(char *buffer, int pixels) {
  int result;
  int i;
  unsigned long size = htonl(pixels);
  write(fd, &size, sizeof(unsigned long));
  printf("Writing pixels: %d\n", pixels);
  for (i=0; i<pixels; i++) {
    result = write(fd, buffer, 3);
    buffer+=3;
    if (result == -1) {
        perror("Error writing to I2C");
    }
  }

  /*
  int i,j;
  for (i=0; i<pixels; i++) {
    for (j=0; j<3; j++) {
      result = wiringPiI2CWrite(fd, *buffer);
      //result = wiringPiI2CWriteReg16(fd, 0x40, (i & 0xfff) );
      if (result == -1) {
        perror("Error writing to I2C");
      }
      buffer++;
    }
  }*/
  
}
