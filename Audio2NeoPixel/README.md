In order to compile the solution, some pre-reqs must be installed.  Specifically, the following packages must
be installed using apt-get.

* asound - libasound2-dev, libasound2
* fftw3 - libfftw3-dev, livfftw3-bin
* websockets - libwebsockets-dev, libwebsockets3
* json-c - libjson-dev, libjson-c2

The `Makefile` included in the package can be used to build the executable.

The solution uses an Arduino to drive the NeoPixel string.  The sketch for the Arduino is found in `arduino.ino`.  The
Arduino appears as an I2C slave device on I2C address `0x10`.

The command to start listening for incoming audio data and driving the NeoPixels is called `audio2neopixel`.  It has some command
line flags:

* `-D device_name` - The identity of the audio device to capture data from.  The list of possible devices can
be found by running with the `-L` flag.
* `-g group_size` - The size (group length) of NeoPixels.  The default is 11.
* `-p port_number` - The port number to listen on for Web Socket data requests.  The default is 3000.
* `-L` - List the audio devices available.  The program ends after listing the information.
* `-?` - Show a help/usage message.

## Architecture
The application is written in C for compilation on the Raspberry Pi / Raspbian.  The core source file
is `audio2neopixel.c` with the other source files being encapsulators for other parts of function.

At a high level, the application uses Linux ALSA to capture audio from the input source.  The audio is captured
in intervals for a number of frames (samples).  Once a capture stream of frames has been received, the data is
passed through a Fourier Transform to split it apart into its constituent frequencies.  These are then normalized
and the number of samples averaged into a smaller set of groups.  The scaling of the groups is logarithmic.

Finally, each group frequency and amplitude is converted into a NeoPixel color value.  A color value is the amount
of Red/Green/Blue.