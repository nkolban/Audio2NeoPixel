# Makefile for audio2neopixel
# https://github.com/nkolban/Audio2NeoPixel
#
AUDIO2NEOPIXEL_OBJS=audio2neopixel.o processData.o webSocket.o json.o alsa.o i2c.o neoPixel.o
AUDIO2NEOPIXEL_LIBS=-lasound -lfftw3 -lm -lwebsockets -ljson-c -lwiringPi
CFLAGS=-g

all: audio2neopixel


audio2neopixel: ${AUDIO2NEOPIXEL_OBJS}
	$(CC) $(CFLAGS) -o $@ $(AUDIO2NEOPIXEL_OBJS) $(AUDIO2NEOPIXEL_LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f audio2neopixel *.o
