/**
 * Process an ALSA audio input source such as a line-in or microphone.
 * Decode the audio data and break it apart into its frequency spectrum.
 * Make the data available through a WebSocket connection.
 */
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "alsa.h"
#include "processData.h"
#include "webSocket.h"
#include "json.h"
#include "neoPixel.h"

#define PCM_DEVICE "plughw:CARD=Device,DEV=0"

int main(int argc, char **argv) {
  int sendsPerSecond = 10;
	unsigned int rc = 44100;
	int          rate = 44100;
  int          channels = 1;
	int          frames = 1024;
	short        *alsaCaptureBuffer;
	size_t       alsaCaptureBufferSize;
  int          end = 0; // Flag to indicate end of processing.
  int          groupSize = 11;
  double       *groupData;
  double       *result;
  char         *jsonString;
  struct timeval lastSend;
  struct timeval now;
  struct timeval delta; 
  struct processData_t processDataInstance; 
  double sampleDuration = frames/rate;
  if (1.0/sendsPerSecond < (double)frames/rate) {
    printf("Can't capture %d frames at %d rate send %d times per second\n", frames, rate, sendsPerSecond);
    return;
  }
	if (argc < 1) {
		printf("Usage: %s\n", argv[0]);
		return -1;
  }
  
  printf("rate=%d, channels=%d, frames in sample=%d\n", rate, channels, frames);
  
  //processData_dumpLogRanges(groupSize);
  websocket_init();
  neopixel_init();
  alsa_init(PCM_DEVICE,channels, rate, frames);

	alsaCaptureBufferSize = frames * channels * sizeof(short);
	alsaCaptureBuffer = (short *) malloc(alsaCaptureBufferSize);

  processData_init(&processDataInstance, frames);
  printf("Processing ...\n");
  
  lastSend.tv_sec = 0;
  lastSend.tv_usec = 0;

  while(!end) {
    rc = alsa_capture(alsaCaptureBuffer);
    if (rc < 0) {
      printf("ERROR: Can't read data. %s\n", snd_strerror(rc));
    }
	
    gettimeofday(&now, NULL);
    timersub(&now, &lastSend, &delta);
    if ((unsigned long)(delta.tv_sec * 1000 + delta.tv_usec/1000) >= 1000 / sendsPerSecond) {
      lastSend = now;
      result = processData_process(&processDataInstance, (short *)alsaCaptureBuffer, rate, channels, frames);	
      // Process the data ...      
      groupData = processData_logGroup(result, rate/2, frames/2, groupSize);
      if (groupData != NULL) {
        neopixel_process(groupData, groupSize);
        jsonString = buildJSONArray(groupData, groupSize);
        webSocket_send(jsonString, strlen(jsonString));
        free(groupData);
        free(jsonString);
      }
    } // End of while !end
    webSocket_service();
  }
  printf("Ending!");
	
	processData_end(&processDataInstance);

	// Close up the usage of the ALSA capture resources.
  alsa_end();
	free(alsaCaptureBuffer);
	
	printf("End of program.\n");
	return 0;
} // End of main
