/**
 * @github https://github.com/nkolban/Audio2NeoPixel
 */
void alsa_init(char *device, int channels, int rate, int iFrames);
int alsa_capture(short *buffer);
void alsa_end();
