#include <fftw3.h>

struct processData_t {
	fftw_complex *in;
	fftw_complex *out;
	fftw_plan plan;
};

void processData_init(struct processData_t *pProcessData, int numItems);
void processData_end(struct processData_t *pProcessData);
double *processData_process(struct processData_t *pProcessData,
  short *data, int sampleRate, int channels, int numItems);
double *processData_ffwGroup(double *data, int frameSize, int groupSize);	
double *processData_logGroup(double *rawData, int rate, int frames, int groupSize);
void processData_dumpLogRanges(int groupSize);
double *processData_getLogRanges(int groupSize);
