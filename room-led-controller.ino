#include <FastLED.h>
#include <Wire.h>

#define NUM_LEDS 150
#define PIN 3
#define LEN 8

// twinkle vars
#define DECREMENT_BY 4
#define INC_BY 150
#define HUE_MIN 205
#define HUE_MAX 220
int randomNumber;

CRGB leds[NUM_LEDS];
int brightness[NUM_LEDS];
int hue[NUM_LEDS];
boolean on = false;
int inc = 0;
int hueInc = 0;

int ledPin = 13;                // choose the pin for the LED
int inputPin = 12;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;
int timeLeft = 0;
//static_assert(LOW == 0, "Expecting LOW to be 0");
int wirelessInput = 0;

void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    wirelessInput = Wire.read(); // receive byte as a character
    Serial.println(wirelessInput);
  }
}

void setup() {
  Wire.begin(0x8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent);
  LEDS.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(255);
  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // turn it off
}

void loop() {

  if (wirelessInput == 0) {
    runway();
  }
  else if (wirelessInput < 255) {
    solid();
  } else {
    decrementBrightness();
    incrementRandom();
  }

  delay(25);
  FastLED.show();
}

void runway() {
  timeLeft--;
  if (inc > LEN) {
    on = !on;
    inc = 0;
    hueInc = 210; // 97 for red
  } else {
    inc++;
    hueInc += 3;
  }

  for (int a = NUM_LEDS - 1; a > 0; a--) {
    hue[a] = hue[a - 1];

    if (brightness[a - 1] == 255) {
      brightness[a] = 255;
    }
    else {
      brightness[a] = 0;
    }

  }

  if (on) {
    brightness[0] = 255;
    hue[0] = hueInc;
  }
  else {
    brightness[0] = 0;
  }

  // fill leds with determined pattern
  for (int x = 0; x < NUM_LEDS; x++) {
    leds[x] = CHSV(hue[x], 255, brightness[x]);
  }
}

void solid() {
  for (int x = 0; x < NUM_LEDS; x++) {
    leds[x] = CHSV(wirelessInput, 255, 255);
  }
}

void clearLeds() {
  for (int x = 0; x < NUM_LEDS; x++) {
    brightness[x] = 0;
    leds[x] = CHSV(0, 0, 0);
    hue[x] = 0;
  }
}

void decrementBrightness() {
  for (int x = 0; x < NUM_LEDS; x++) {
    if (brightness[x]) {
      if (brightness[x] - DECREMENT_BY < 0) {
        brightness[x] = 0;
        leds[x] = CHSV(0, 0, 0);
      } else {
        brightness[x] -= DECREMENT_BY;
        leds[x] = CHSV(hue[x], 255, brightness[x]);
      }
    }
  }
}

void incrementRandom() {
  randomNumber = random(0, NUM_LEDS - 1);
  brightness[randomNumber] += INC_BY;
  hue[randomNumber] = random(HUE_MIN, HUE_MAX);
  if (brightness[randomNumber] > 255) {
    brightness[randomNumber] = 255;
  }
  leds[randomNumber] = CHSV(hue[randomNumber], 255, brightness[randomNumber]);
}
