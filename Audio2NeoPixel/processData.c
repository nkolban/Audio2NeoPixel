/**
 * Encapsulate the idea of processing audio data.
 * First we make a call to initProcessData passing in an instance of struct processData_t which
 * will maintain our state.  We also pass in the number of frames in a sample.
 * 
 * When we are finished with performing processing, we can call endProcessData to release any
 * resources that were allocated.
 * 
 * Once we have initialized our process data we can then call processData() to process an audio
 * sample.  The signature of this is:
 * processData(
 *  struct processData_t *pProcessData - The state of our processing.
 *  short *data - The data we are about to process.
 *  int sampleRate - The sample rate of the data we are processing.
 *  int channels - The number of channels in our data.
 *  int numItems - The number of samples in the data.
 * following a call to processData, the return array of doubles has been
 * populated with a malloced memory buffer which is 'numItems' in size.  The data contained within
 * contains the magnitude of the frequency.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h> 
#include "processData.h"

static void processData_normalize(double *data, int size);

void processData_init(struct processData_t *pProcessData, int numItems) {
	pProcessData->in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * numItems);
	pProcessData->out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * numItems);	
	pProcessData->plan = fftw_plan_dft_1d(numItems, pProcessData->in, pProcessData->out, FFTW_FORWARD, FFTW_ESTIMATE);
}

void processData_end(struct processData_t *pProcessData) {
	fftw_destroy_plan(pProcessData->plan);
	fftw_free(pProcessData->in);
	fftw_free(pProcessData->out);
	pProcessData->plan = NULL;
	pProcessData->in = NULL;
	pProcessData->out = NULL;
}

double *processData_process(struct processData_t *pProcessData, short *data, int sampleRate, int channels, int numItems) {
	int i;
  double *result;
	for (i=0; i<numItems; i++) {
		pProcessData->in[i][0] = data[i*channels];
		pProcessData->in[i][1] = 0;
	}
	
	fftw_execute(pProcessData->plan);
	//printf("fft plan executed!\n");
	
	// Lets build an output array
	result = (double *)malloc(numItems * sizeof(double));
	for (i=0; i<numItems; i++) {
		result[i] = sqrt(pow((double)pProcessData->out[i][0], 2) + pow((double)pProcessData->out[i][1], 2));
	}
	// At this point, we have an array of size 'numItems' of doubles.  Each entry in the array
	// corresponds to the energy of a specific frequency.  Each element in the array differs from
	// the last by a fequency of sampleRate/numItems.
  return result;
}

/**
 * Create a smaller array of groupSize containing the average energy for the given group size
 * entries.
 */
double *processData_ffwGroup(double *data, int frames, int groupSize) {
	if (frames % groupSize != 0) {
		printf("ffwGroup: frames=%d, groupSize=%d ... frames should be evenly divisible by groupSize\n", frames, groupSize);
		return NULL;
	}
	int stepSize = frames/groupSize;
	int i;
	double *result = (double *)malloc(sizeof(double) * groupSize);
	for (i=0; i<groupSize; i++) {
		double accumulator = 0.0;
		int k;
		for (k=0; k<stepSize; k++) {
			accumulator += *data;
			data++;
		}
		result[i]=accumulator/stepSize;
	}
  //processData_normalize(result, groupSize);
	return result;
}

// start - start frequency
// stop - stop frequency
// n - the point which you wish to compute (zero based)
// N - the number of points over which to divide the frequency
// range.
static double logspace(double start, double stop, int n, int N)
{
    return start * pow(stop/start, n/(double)(N-1));
}

/**
 * rawData is an array of energies for frequencies.
 * frames is the number of entries in rawData.
 * Since audio goes from 0HZ - rate (44.1KHz/2) ... each entry in the rawData
 * is rate/frames of frequency span.  To create out logarithmic results, we need
 * to average the data into groupSize elements.  See:
 * http://stackoverflow.com/questions/32320028/convert-linear-audio-frequency-distribution-to-logarithmic-perceptual-distributi
 */
double *processData_logGroup(double *rawData, int rate, int frames, int groupSize) {
  double *result = (double *)malloc(sizeof(double) * groupSize);
  double spacing = (double)(rate)/frames;
  double accumulator = 0.0;
  int counter = 0;
  int logGroup = 0;
  double next = logspace(50, 22000, logGroup, groupSize);
  int i;
  for (i=0; i<frames; i++) {
    accumulator += rawData[i];
    counter++;
    if ((i * spacing) > next) {
      result[logGroup] = accumulator/counter;
      accumulator = 0.0;
      counter = 0;
      logGroup++;
      next = logspace(50, 22000, logGroup, groupSize);
    }
  }
  return result;
} // End of processData_logGroup

void processData_dumpLogRanges(int groupSize) {
  int i;
  for (i=0; i<groupSize; i++) {
    printf("Group: %d, logspace: %f\n", i, logspace(50,22000, i, groupSize));
  }
}

double *processData_getLogRanges(int groupSize) {
  double *retData = malloc(sizeof(double) * groupSize);
  int i;
  for (i=0; i<groupSize; i++) {
    retData[i] = logspace(50,22000, i, groupSize);
  }
  return retData;
}

static void processData_normalize(double *data, int size) {
  double largest = 0;
  int i;
  for (i=0; i<size; i++) {
    if (data[i] > largest) {
      largest = data[i];
    }
  }
  for (i=0; i<size; i++) {
    data[i] = floor(255 * data[i] / largest);
  }
}
