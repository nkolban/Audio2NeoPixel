/**
 * Process an ALSA audio input source such as a line-in or microphone.
 * Decode the audio data and break it apart into its frequency spectrum.
 * Make the data available through a WebSocket connection.
 *
 * @github https://github.com/nkolban/Audio2NeoPixel
 */
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include "alsa.h"
#include "processData.h"
#include "webSocket.h"
#include "json.h"
#include "neoPixel.h"

#define PCM_DEVICE "plughw:CARD=Device,DEV=0"

/**
 * Name of the pcmDevice.
 */
static char *pcmDevice = NULL;

/**
 * Should we list devices?
 */
static int listDevices = 0;

/**
 * The number of items in a group (pixel string size)
 */
static int groupSize = 11;

/**
 * Port number for Web socket
 */
static unsigned short portNumber = 3000;

/**
 * Handle the parsing options.
 */
static int parse_opt(int key, char *arg, struct argp_state *state) {
  switch(key) {
  case 'D':
    pcmDevice = arg;
    break;

  case 'L':
    listDevices = 1;
    break;

  case 'g':
    groupSize = atoi(arg);
    if (groupSize <= 0) {
      printf("Invalid group size.\n");
      return 1;
    }
    break;

  case 'p':
    portNumber = atoi(arg);
    if (portNumber == 0) {
      printf("Invalid port number.\n");
      return 1;
    }
    break;
  }

  return 0;
} // End of parse_opt


int main(int argc, char **argv) {
  int    sendsPerSecond = 10;
	int    rc;
	int    rate = 44100;
  int    channels = 1;
	int    frames = 1024;
	short  *alsaCaptureBuffer;
	size_t alsaCaptureBufferSize;
  int    end = 0; // Flag to indicate end of processing.

  double *groupData;
  double *result;
  char   *jsonString;

  struct timeval lastSend;
  struct timeval now;
  struct timeval delta; 
  struct processData_t processDataInstance; 

  double sampleDuration = frames/rate;
  if (1.0/sendsPerSecond < (double)frames/rate) {
    printf("Can't capture %d frames at %d rate send %d times per second\n", frames, rate, sendsPerSecond);
    return;
  }


  /**
   * Parse the options
   */
  struct argp_option options[] = {
      {"device",    'D', "device_name", 0, "Specify an audio input device"},
      {"list",      'L', 0,             0, "List audio capture devices"},
      {"groupsize", 'g', "group_size",  0, "The size of the group of results"},
      {"port",      'p', "port_number", 0, "Web socket listener port (default=3000)"},
      {0}
  };
  struct argp argp = {options, parse_opt};
  rc = argp_parse(&argp, argc, argv, 0, 0, 0);
  if (rc != 0) {
    return rc;
  }

  /**
   * If the option flag asked us to list input devices, then list them and end.
   */
  if (listDevices) {
    alsa_listInputDevices();
    return 0;
  }
  
  /**
   * We must have a valid device name supplied via the -D flag.
   */
  if (pcmDevice == NULL) {
    printf("No device name supplied.\n");
    exit(1);
  }

  printf("device=%s, rate=%d, channels=%d, frames in sample=%d, groupSize=%d\n", pcmDevice, rate, channels, frames, groupSize);
  alsa_init(pcmDevice, channels, rate, frames);
  websocket_init(portNumber);
  neopixel_init();

  processData_init(&processDataInstance, frames);

  printf("Processing ...\n");
  
  lastSend.tv_sec = 0;
  lastSend.tv_usec = 0;
  alsaCaptureBufferSize = frames * channels * sizeof(short);
  alsaCaptureBuffer = (short *) malloc(alsaCaptureBufferSize);

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
      // Group the data into a logarithmic scale result set
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
