#include <stdio.h>
#include <sndfile.h>
#include <string.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>

int main(int argc, char **argv[]) {
	SF_INFO info;
	SNDFILE *file;
	if (argc != 2) {
		printf("Usage: %s fileName\n", argv[0]);
		return -1;
	}
	memset(&info, 0, sizeof(info));
	file = sf_open((const char *)argv[1], SFM_READ, &info);
	if (file == NULL) {
		printf("Error: %s\n", sf_strerror(file));
		return -1;
	}
	printf("Working of file %s\n", argv[1]);
	printf(" - frames: %d\n", info.frames);
	printf(" - samplerate: %d\n", info.samplerate);
	printf(" - channels: %d\n", info.channels);
	printf(" - format: %x\n", info.format);
	switch(info.format & SF_FORMAT_TYPEMASK) {
		case SF_FORMAT_WAV: 
			printf("  - WAV\n");
			break;
		default:
			printf("  - Unknown format\n");
			break;
	}
	switch(info.format & SF_FORMAT_SUBMASK) {
		case SF_FORMAT_PCM_S8:
			printf("  - Signed 8 bit\n");
			break;
		case SF_FORMAT_PCM_16:
			printf("  - Signed 16 bit\n");
			break;
		default:
			printf("  - Unknown sub-format\n");
			break;
	}
	switch(info.format & SF_FORMAT_ENDMASK) {
		case SF_ENDIAN_FILE:
			printf("  - Default file endian\n");
			break;
		case SF_ENDIAN_LITTLE:
			printf("  - Little endian\n");
			break;		
		case SF_ENDIAN_BIG:
			printf("  - Big endian\n");
			break;	
		default:	
			printf("  - Unknown endian\n");
			break;		
	}
	
	printf(" - sections: %d\n", info.sections);
	printf(" - seekable: %d\n", info.seekable);
	
	// 2^12=4096
	// 2^13=8192
	// 2^14=16384
	// 2^15=32768
	int items = pow(2,15);
	short *buffer = (short *)malloc(sizeof(short) * items * 2);
	if (buffer == 0) {
		printf("Error: Failed to allocate storage\n");
		return -1;
	}
	
	sf_seek(file, 2000000, SEEK_SET);
	sf_count_t count;
	count = sf_read_short(file, buffer, items*2);
	printf("Read %d items\n", (int)count);
	if (count == 0) {
		printf("Error: %s\n", sf_strerror(file));
	}
	
	fftw_complex *in, *out;
	fftw_plan plan;
	
	in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * items);
	out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * items);	
	plan = fftw_plan_dft_1d(items, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	
	int i;
	for (i=0; i<items; i++) {
		in[i][0] = buffer[i*2];
		in[i][1] = 0;
	}
	
	fftw_execute(plan);
	printf("fft plan executed!\n");
	/*
	printf("Let's look at the input data ...\n");
	for (i=0; i<items; i++) {
		printf("[%d %d] ", buffer[i*2], buffer[i*2+1]);
	}
	printf("Let's look at the data ...\n");
	for (i=0; i<items; i++) {
		printf("[%f %f] ", out[i][0], out[i][1]);
	}
	*/
	
	// Lets build an output array
	double *outputData;
	outputData = (double *)malloc(items * sizeof(double));
	for (i=0; i<items; i++) {
		outputData[i] = sqrt(pow((double)out[i][0], 2) + pow((double)out[i][1], 2));
	}
	FILE *outFile = fopen("data.dat", "w");
	for (i=0; i<(items/2); i++) {
		fprintf(outFile, "%d %f\n", i*info.samplerate/items, outputData[i]);
	}
	fclose(outFile);
	fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);
	
	// Let us now try an FFT
	sf_close(file);
	free(buffer);
}
