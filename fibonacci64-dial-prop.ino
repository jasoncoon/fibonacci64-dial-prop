/*
   Fibonacci64 Dial Prop: https://github.com/jasoncoon/fibonacci64-dial-prop
   Copyright (C) 2020 Jason Coon
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Board: Adafruit Feather 32u4
// Libraries:

#include <Button.h>        // https://github.com/madleech/Button
#include <RotaryEncoder.h> // https://github.com/mathertel/RotaryEncoder
#include <FastLED.h>       // https://github.com/FastLED/FastLED

FASTLED_USING_NAMESPACE

#define DATA_PIN    6
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    64
CRGB leds[NUM_LEDS];

#define MILLI_AMPS         1500
#define BRIGHTNESS         64
#define FRAMES_PER_SECOND  60

RotaryEncoder encoder(1, 0);

Button button = Button(10);

#include "gradientPalettes.h"
#include "map.h"

bool isShowingDial = true;

bool startedTravelling = true;
uint8_t travellingSpeed = 1;

uint8_t dialOffsetAngle = 12;
uint8_t dialStartAngle = 0;
uint8_t dialEndAngle = dialOffsetAngle;

uint8_t dialStartRadius = 0;
uint8_t dialEndRadius = 255;

const uint8_t encoderPositions = 13;
const uint8_t angleIncrement = 255 / encoderPositions;

const uint8_t idlingPosition = encoderPositions - 1;
const uint8_t glitchingPosition = encoderPositions - 2;

void setup() {
  Serial.begin(115200);

  //  delay(1000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  FastLED.setCorrection(UncorrectedColor); // TypicalSMD5050, Typical8mmPixel, UncorrectedColor
  FastLED.setDither(false);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);

  button.begin();

  encoder.setPosition(0);
}

void loop()
{
  if (button.released()) {
    isShowingDial = !isShowingDial;
    startedTravelling = !isShowingDial;
  }

  static int encoderPosition = 0;

  if (isShowingDial) {
    drawDial(dialStartAngle, dialEndAngle);

    encoder.tick();
    int newEncoderPosition = encoder.getPosition() % encoderPositions;
    if (newEncoderPosition < 0) newEncoderPosition += encoderPositions;
    if (newEncoderPosition != encoderPosition) {
      encoderPosition = newEncoderPosition;

      dialEndAngle = (encoderPosition * angleIncrement) + dialOffsetAngle; // make the dial rotation match the encoder rotation

      encoder.setPosition(encoderPosition);

      Serial.print("dialStartAngle: ");
      Serial.print(dialStartAngle);
      Serial.print(", dialEndAngle: ");
      Serial.print(dialEndAngle);
      Serial.print(", position: ");
      Serial.println(newEncoderPosition);
    }
  }
  else if (encoderPosition == idlingPosition) {
    drawIdlingAnimation();
  }
  else if (encoderPosition == glitchingPosition) {
    drawGlitchingAnimation();
  }
  else {
    drawTravellingAnimation();
  }

  FastLED.show();
}

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0) {
  uint8_t beat = beat8( beats_per_minute, timebase);
  uint8_t beatsaw = beat + phase_offset;
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8( beatsaw, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

void drawDial(uint8_t dialStartAngle, uint8_t dialEndAngle) {
  fadeToBlackBy(leds, NUM_LEDS, 32);

  const uint8_t dialHue = 144;
  const uint8_t dialSaturation = 232;
  const uint8_t dialValue = 111;
  const CHSV dialColor = CHSV(dialHue, dialSaturation, dialValue);

  const uint8_t indicatorPulseBPM = 60;
  const uint8_t indicatorHue = 144; // blue
  const uint8_t indicatorSaturation = 0; // completely desaturated (white)
  const uint8_t indicatorMinValue = 64;
  const uint8_t indicatorMaxValue = 255 - dialValue;
  uint8_t indicatorValue = beatsin8(indicatorPulseBPM, indicatorMinValue, indicatorMaxValue);
  CHSV indicatorColor = CHSV(indicatorHue, indicatorSaturation, indicatorValue);

  drawArc(dialStartAngle, dialEndAngle, dialStartRadius, dialEndRadius, dialColor);
  antialiasPixels(dialEndAngle - 8, 8, 0, dialEndRadius, indicatorColor);
}

/*
  Built-in palettes:
  RainbowColors_p,
  RainbowStripeColors_p,
  CloudColors_p,
  LavaColors_p,
  OceanColors_p,
  ForestColors_p,
  PartyColors_p,
  HeatColors_p
*/

/*
   Bluish gradient palettes
   2: es_ocean_breeze_036_gp
   5: error?
   7: Coral_reef_gp
   8: es_ocean_breeze_068_gp
*/

void drawIdlingAnimation() {
  const CRGBPalette16 palette = palettes[2];

  const uint8_t speed = 30;

  radiusPalette(palette, speed);
}

void radiusPalette(CRGBPalette16 palette, uint8_t speed) {
  uint8_t hues = 1;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    uint8_t r = physicalToFibonacci[i];

    leds[i] = ColorFromPalette(palette, beat8(speed) - (r * hues));
  }
}

DEFINE_GRADIENT_PALETTE( Red_gp ) {
  0,  255,  0,  0, // Red
  255,  0,  0,  0 // Black
};

void drawGlitchingAnimation() {
  drawTravellingAnimation();

  const fract8 chanceOfGlitch = 15;

  fadeToBlackBy(leds, NUM_LEDS, 240);

  if (random8() < chanceOfGlitch) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    const CRGBPalette16 palette = Red_gp;

    const uint8_t speed = 120;

    anglePalette(palette, speed, 3);
  }
}

// attempt at a custom palette
DEFINE_GRADIENT_PALETTE( Cherenkov_gp ) {
  0,   222, 239, 249, // Alice Blue
  1,   172, 220, 240, // Non Photo Blue
  2,    41, 163, 212, // Cerulean Crayola
  4,    10, 110, 183, // Spanish Blue
  16,   10,  84, 149, // USAFA Blue
  160,   6,   7,  15, // Rich Black FOGRA 39
  255,   0,   0,   0  // Black
};

void drawTravellingAnimation() {
  static uint8_t paletteIndex = 7; // Coral_reef_gp

  //  // code for exploring other palettes
  //  EVERY_N_SECONDS(5) {
  //    paletteIndex++;
  //    if (paletteIndex >= paletteCount) paletteIndex = 0;
  //    Serial.print("paletteIndex: ");
  //    Serial.println(paletteIndex);
  //  }

  const CRGBPalette16 palette = palettes[paletteIndex];

  //  const CRGBPalette16 palette = Cherenkov_gp;

  static bool warmingUp = false;

  static uint8_t endAngle = 0;

  if (startedTravelling) {
    endAngle = dialEndAngle;
    warmingUp = true;
    startedTravelling = false;
    travellingSpeed = 4;
  }

  if (warmingUp) {
    drawDial(0, endAngle);

    if (endAngle <= 253)
      endAngle += 2;
    else
      warmingUp = false;
  }
  else {
    static uint8_t offset = 1;

    EVERY_N_MILLIS(1000) {
      if (travellingSpeed < 8)
        travellingSpeed++;
    }

    EVERY_N_MILLIS(30) {
      offset += travellingSpeed;
    }

    anglePalette(palette, offset, 3);
  }
}

void anglePalette(CRGBPalette16 palette, uint8_t offset, uint8_t hues) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    uint8_t a = angles[i];

    leds[i] = ColorFromPalette(palette, ((a + offset) * hues));
  }
}
