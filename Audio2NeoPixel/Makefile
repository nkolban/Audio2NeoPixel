RECORDER_OBJS=recorder.o processData.o webSocket.o json.o alsa.o i2c.o neoPixel.o
RECORDER_LIBS=-lasound -lfftw3 -lm -lwebsockets -ljson-c -lwiringPi
CFLAGS=-g

all: player sndfiletest recorder


player: player.c 
	$(CC) -o player player.c  -lasound

recorder: ${RECORDER_OBJS}
	$(CC) $(CFLAGS) -o $@ $(RECORDER_OBJS) $(RECORDER_LIBS)

sndfiletest: sndfiletest.c processData.o
	cc -o sndfiletest sndfiletest.c processData.o -lsndfile -lfftw3 -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f player sndfiletest recorder *.o
