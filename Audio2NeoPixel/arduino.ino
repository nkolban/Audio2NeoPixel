#include <Adafruit_NeoPixel.h>

#include <Wire.h>

#define DEVICE_ADDRESS (0x10)
#define NUM_PIXELS (11)
static unsigned long numPixels = 0;
static unsigned long currentPixel = 0;
static unsigned char *pixelBuffer = NULL;
static Adafruit_NeoPixel strip;

void receiveHandler(int numBytes) {
  Serial.print("receiveHandler - We have received some data: ");
  Serial.println(numBytes);
  int i;
  if (numBytes == 4) {
    // we got an integer.
    numPixels = 0;
    for (i = 0; i<4; i++) {
      numPixels = (numPixels <<8) + Wire.read();
    }
    Serial.print("Num of pixels ...");
    Serial.println(numPixels);
    currentPixel = 0;
    if (pixelBuffer != NULL) {
      free(pixelBuffer);
    }
    pixelBuffer = (unsigned char *)malloc(3 * numPixels);
  } else {
    for (i=0; i<numBytes; i++) {
      pixelBuffer[currentPixel*3 + i] = Wire.read();
    }
    currentPixel++;
    if (currentPixel == numPixels) {
      // We have all out pixels ...
      Serial.println("Sending pixels to NeoPixel stream ...");
      for (i=0; i<numPixels; i++) {
        strip.setPixelColor(i, strip.Color(pixelBuffer[i*3], pixelBuffer[i*3+1], pixelBuffer[i*3+2]));
      }
      strip.show();
    }
  }
} // End of receive handler

void requestHandler() {
  Serial.print("requestHandler - We need to return some data\n");
  Wire.write(0x99);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(DEVICE_ADDRESS);
  Wire.onReceive(receiveHandler);
  Wire.onRequest(requestHandler);
  strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_RGB + NEO_KHZ800);
  strip.begin();
  Serial.println("Ready! We are now able to start receiving I2C data.");
}

void loop() {
}