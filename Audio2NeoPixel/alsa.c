/**
 * Encapsulate access to the ALSA sound interface.
 * @github https://github.com/nkolban/Audio2NeoPixel
 */
#include <alsa/asoundlib.h>
static snd_pcm_t *pcm_handle;
static snd_pcm_uframes_t frames;
static snd_pcm_hw_params_t *params;

/**
 * Capture an array of data.
 */
int alsa_capture(short *buffer) {
  int rc = snd_pcm_readi(pcm_handle, buffer, frames);
  return rc;
} // End of alsa_capture


/**
 * Initialize our interface to ALSA.
 */
void alsa_init(
  char *device,
  int channels, // The number of channels we are working with (usually 1 or 2)
  int rate,     // The capture rate (usually 44100)
  int iFrames)  // The number of frames in a sample.
{

  unsigned int dir;
  unsigned int rc;
 
  frames = (snd_pcm_uframes_t)iFrames;

  /* Open the PCM device in record mode */
	if (rc = snd_pcm_open(&pcm_handle, device,	SND_PCM_STREAM_CAPTURE, 0) < 0) { 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",	device, snd_strerror(rc));
	}

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (rc = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(rc));
	}

	if (rc = snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE) < 0) {
		printf("ERROR: Can't set format. %s\n", snd_strerror(rc));
	}

	if (rc = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) {
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(rc));
	}

	if (rc = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) {
		printf("ERROR: Can't set rate. %s\n", snd_strerror(rc));
	}
	
	if (rc = snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir) < 0) {
		printf("ERROR: Can't set period size. %s\n", snd_strerror(rc));
	}

	/* Write parameters */
	if (rc = snd_pcm_hw_params(pcm_handle, params) < 0) {
		printf("ERROR: Can't set hardware parameters. %s\n", snd_strerror(rc));
	}
} // End of alsa_init


/**
 * Close and cleanup our ALSA interactions.
 */
void alsa_end() {
  snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
} // End of alsa_end
