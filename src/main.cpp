#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <bitmaps.h>

#define PIR_INPUT 1
bool interrupt = LOW;

#define RELAY_OUT 0
#define TEST_BUTTON 17

#define LED_STRIP_PIN 6
#define MATRIX_WIDTH 9
#define MATRIX_HEIGHT 18

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoMatrix strip = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, LED_STRIP_PIN,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_RGB            + NEO_KHZ800);

#define BLACK   0x0000
#define WHITE   0xFFFF
#define YELLOW  0xFFE0
#define BRIGHTNESS 255

int textSpeed = 200;
int flockSpeed = 120; // ms between frames

void movmentSensed() {
  interrupt = HIGH;
  digitalWrite(LED_BUILTIN, interrupt);
}

void nonBlockingDelay(int del) {
  unsigned long myPrevMillis = millis();
  while (millis()- myPrevMillis <= del);
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void pixelsTest(){
    digitalWrite(RELAY_OUT, HIGH);
    strip.fillScreen(0);
    strip.show();
    nonBlockingDelay(1000);
    for (int j = 0; j < 18; j++){
        strip.drawLine(0,j,9,j,strip.Color(100,100,100));
        strip.show();
        nonBlockingDelay(50);
    }
    nonBlockingDelay(2000);
    rainbow(10);
}

void displayMan(){
    strip.fillScreen(BLACK);
    strip.drawBitmap(0, 0, man, 9, 18, WHITE);
    strip.show();
}

void flockAnimation() {
    int b = BRIGHTNESS;
    strip.fillScreen(BLACK);
    for (int a = 1; a < 36; a++){
        strip.drawBitmap(0, 0, bitmapFrames[a], 9, 18, WHITE);
        strip.setBrightness(b - (a*2));
        strip.show();
        nonBlockingDelay(flockSpeed);
        strip.fillScreen(BLACK);
    }
    strip.setBrightness(BRIGHTNESS);
}

void switchToFlock(){
    displayMan();
    nonBlockingDelay(3000);
    digitalWrite(RELAY_OUT, HIGH);
    interrupt = LOW;
    flockAnimation();
    digitalWrite(LED_BUILTIN, interrupt);
    nonBlockingDelay(1000);
    // digitalWrite(RELAY_OUT, LOW);
}

void scrollText(){
    for (int i = 8; i  > -(6*12); i--){
        strip.fillScreen(BLACK);
        strip.setCursor(i, 18);

        if (i < -15){
            digitalWrite(RELAY_OUT, LOW);
        };
        if ((interrupt && (i == 8))){
            i = 8;
            switchToFlock();
        }

        strip.print(F("Poland"));
        strip.show();
        nonBlockingDelay(textSpeed);
    }
}

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.setTextColor(WHITE);
    strip.setTextWrap(false);

    // strip.setFont(&FreeMono12pt7b);
    strip.setFont(&FreeSans12pt7b);
    // strip.setFont(&FreeSerif12pt7b);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, interrupt);
    pinMode(TEST_BUTTON,INPUT_PULLUP);

    pinMode(PIR_INPUT, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PIR_INPUT), movmentSensed, RISING);
    pinMode(RELAY_OUT, OUTPUT);
    digitalWrite(RELAY_OUT, LOW);

    if (!digitalRead(TEST_BUTTON)){
        pixelsTest();
    }
}

void loop() {
    scrollText();
}
