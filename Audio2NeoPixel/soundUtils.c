#include <stdio.h>
#include <sndfile.h>
#include "soundUtils.h"
static SNDFILE *soundFile = NULL;

void soundFile_startSave(char *fileName, int frames) {
  SF_INFO sfinfo;
  sfinfo.samplerate = 44100;
  sfinfo.channels = 1;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_FILE;
  int mode = SFM_WRITE;
  soundFile = sf_open(fileName, mode, &sfinfo);
  if (soundFile == NULL) {
    printf("Error: %s\n", sf_strerror(soundFile));
  }
}

void soundFile_writeData(short *data, int items) {
  if (soundFile == NULL) {
    return;
  }
  sf_write_short(soundFile, data, items);
}

void soundFile_endSave() {
  if (soundFile == NULL) {
    return;
  }
  sf_write_sync(soundFile);
  sf_close(soundFile);
}
