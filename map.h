uint8_t physicalToFibonacci[NUM_LEDS] = { 0, 13, 26, 39, 52, 57, 44, 31, 18, 5, 10, 23, 36, 49, 62, 54, 41, 28, 15, 2, 7, 20, 33, 46, 59, 51, 38, 25, 12, 4, 17, 30, 43, 56, 61, 48, 35, 22, 9, 1, 14, 27, 40, 53, 58, 45, 32, 19, 6, 11, 24, 37, 50, 63, 55, 42, 29, 16, 3, 8, 21, 34, 47, 60 };
uint8_t fibonacciToPhysical[NUM_LEDS] = { 0, 39, 19, 58, 29, 9, 48, 20, 59, 38, 10, 49, 28, 1, 40, 18, 57, 30, 8, 47, 21, 60, 37, 11, 50, 27, 2, 41, 17, 56, 31, 7, 46, 22, 61, 36, 12, 51, 26, 3, 42, 16, 55, 32, 6, 45, 23, 62, 35, 13, 52, 25, 4, 43, 15, 54, 33, 5, 44, 24, 63, 34, 14, 53 };
uint8_t coordsX[NUM_LEDS] = { 140, 189, 208, 214, 208, 146, 168, 180, 180, 162, 152, 146, 129, 103, 72, 40, 70, 97, 120, 131, 107, 79, 50, 23, 0, 7, 23, 46, 76, 93, 57, 37, 28, 29, 87, 68, 59, 62, 80, 113, 91, 94, 109, 133, 202, 172, 145, 125, 117, 145, 170, 198, 227, 253, 255, 235, 210, 181, 148, 175, 207, 228, 240, 244 };
uint8_t coordsY[NUM_LEDS] = { 128, 114, 91, 63, 34, 0, 21, 48, 76, 106, 78, 47, 25, 11, 5, 38, 35, 42, 61, 101, 87, 69, 68, 78, 98, 143, 118, 102, 98, 122, 131, 152, 179, 209, 255, 230, 202, 174, 148, 142, 181, 210, 235, 252, 235, 234, 224, 203, 170, 183, 201, 205, 198, 181, 134, 157, 171, 173, 153, 145, 138, 120, 93, 63 };
uint8_t angles[NUM_LEDS] = { 0, 249, 241, 232, 223, 200, 208, 217, 226, 235, 212, 203, 194, 185, 176, 162, 171, 180, 188, 197, 174, 165, 156, 147, 139, 124, 133, 142, 151, 136, 128, 119, 110, 101, 78, 86, 95, 104, 113, 99, 90, 81, 72, 63, 40, 49, 58, 67, 75, 52, 43, 34, 25, 17, 2, 11, 20, 29, 38, 14, 6, 255, 246, 237 };

void drawArc(uint8_t startAngle, uint8_t endAngle, uint8_t startRadius, uint8_t endRadius, CRGB color) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t o = i;

    uint8_t ao = 255 - angles[o];

    if (ao <= endAngle && ao >= startAngle) {
      uint8_t ro = physicalToFibonacci[o];

      if (ro <= endRadius && ro >= startRadius) {
        leds[i] += color;
      }
    }
  }
}

void antialiasPixels(uint8_t angle, uint8_t dAngle, uint8_t startRadius, uint8_t endRadius, CRGB color)
{
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t o = i;

    uint8_t ao = 255 - angles[o];

    uint8_t adiff = min(sub8(ao, angle), sub8(angle, ao));
    uint8_t fade = map(adiff, 0, dAngle, 0, 255);
    CRGB faded = color;
    faded.fadeToBlackBy(fade);

    if (adiff <= dAngle) {
      uint8_t ro = physicalToFibonacci[o];

      if (ro <= endRadius && ro >= startRadius) {
        leds[i] += faded;
      }
    }
  }
}
