In order to compile the solution, some pre-reqs must be installed.  Specifically, the following packages must
be installed using apt-get.

* asound - libasound2-dev, libasound2
* fftw3 - libfftw3-dev, livfftw3-bin
* websockets - libwebsockets-dev, libwebsockets3
* json-c - libjson-dev, libjson-c2

The `Makefile` included in the package can be used to build the executable.

The solution uses an Arduino to drive the NeoPixel string.  The sketch for the Arduino is found in `arduino.ino`.  The
Arduino appears as an I2C slave device on I2C address `0x10`.

## Architecture
The application is written in C for compilation on the Raspberry Pi / Raspbian.  The core source file
is `audio2neopixel.c` with the other source files being encapsulators for other parts of function.

At a high level, the application uses Linux ALSA to capture audio from the input source.  The audio is captured
in intervals for a number of frames (samples).  Once a capture stream of frames has been received, the data is
passed through a Fourier Transform to split it apart into its constituent frequencies.  These are then normalized
and the number of samples averaged into a smaller set of groups.  The scaling of the groups is logarithmic.

Finally, each group frequency and amplitude is converted into a NeoPixel color value.  A color value is the amount
of Red/Green/Blue.