void showCircleClock(const DateTime& dt) {
  memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

  uint8_t H = dt.hour();
  uint8_t M = dt.minute();
  uint8_t S = dt.second();

  // we only want to show 12 of 24 hours ..
  H += H < 12 ? 0 : -12;

  float HourLED_1 = NUM_LEDS * (H + ((M + (S / 60.0)) / 60.0)) / 12.0;

  // make a nice alignment of minutes and seconds
  M += M < 58 ? 2 : -58;
  S += S < 58 ? 2 : -58;

  uint8_t MinuteLED = NUM_LEDS * M / 60;
  uint8_t SecondLED = NUM_LEDS * S / 60;
  
  HourLED_1 += HourLED_1 >= 1 ? -1 : (NUM_LEDS-1); // shift 1 to the left
  MinuteLED = MinuteLED > 0 ? MinuteLED - 1 : (NUM_LEDS-1); // shift 1 to the left
  SecondLED = SecondLED > 0 ? SecondLED - 1 : (NUM_LEDS-1); // shift 1 to the left

  uint8_t HourLED_2 = (int8_t)(HourLED_1 < (NUM_LEDS-1) ? HourLED_1 + 1 : HourLED_1 - (NUM_LEDS-1));

#ifdef DEBUG
  Serial.print(dt.hour()); Serial.print(":"); Serial.print(dt.minute()); Serial.print(":"); Serial.print(dt.second());  
  Serial.print(" => "); Serial.print("(");
  Serial.print(HourLED_1); Serial.print(", "); Serial.print(HourLED_2); Serial.print(")");
  Serial.print(" : "); Serial.print(MinuteLED); Serial.print(" : "); Serial.println(SecondLED);
#endif

  leds[(NUM_LEDS-1) - (uint8_t)(HourLED_1)].r = (uint8_t)(255 * (1 - (HourLED_1 - (uint8_t)HourLED_1)));
  leds[(NUM_LEDS-1) - HourLED_2].r = (uint8_t)(255 * (HourLED_1 - (uint8_t)HourLED_1));
  leds[(NUM_LEDS-1) - MinuteLED].g = 255;
  leds[(NUM_LEDS-1) - SecondLED].b = S % 2 == 0 ? 255 : 128;
  
  LEDS.show();
}

