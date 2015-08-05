//#define DEBUG

// http://playground.arduino.cc/Code/time

#include <avr/sleep.h>
#include <avr/power.h>

/*
ISR(TIMER1_OVF_vect) {
  return;
}
*/

// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html
//#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

#include <Wire.h>
#include "RTClib.h"

#include "FastSPI_LED2.h"
#define NUM_LEDS 16
struct CRGB leds[NUM_LEDS];

RTC_DS1307 RTC;
DateTime now, last_now;

int16_t TimeZone;
uint16_t TimeZoneAddr = 1;

boolean BrightnessChanged = false;
uint8_t Brightness = 0x10;
uint8_t NewBrightness = 0x10;

const uint8_t ButtonUpPin = 6;
const uint8_t ButtonDownPin = 5;

uint8_t ButtonUpState = HIGH; // equals to "not pressed"
uint8_t ButtonDownState = HIGH;

uint8_t LastButtonUpState = HIGH;
uint8_t LastButtonDownState = HIGH;

void setup() {
  pinMode(ButtonUpPin, INPUT_PULLUP);
  pinMode(ButtonDownPin, INPUT_PULLUP);

/*
  // is only run once for first EEPROM-init
  while (!eeprom_is_ready());
  eeprom_write_word((uint16_t*)TimeZoneAddr, 7200);
*/

  Serial.begin(9600);

  LEDS.setBrightness(Brightness);
  LEDS.addLeds<WS2812, 4, GRB>(leds, NUM_LEDS);//->clearLeds(300);

  Wire.begin();
  RTC.begin();
  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  last_now = now = DateTime(RTC.now().unixtime() + TimeZone);

  TWBR=12;

  power_adc_disable();
  power_spi_disable();
  power_timer2_disable();
  
  /* Normal timer operation.*/
//  TCCR1A=0x00; 

  /* Clear the timer counter register.
   * You can pre-load this register with a value in order to 
   * reduce the timeout period, say if you wanted to wake up
   * ever 4.0 seconds exactly.
   */
//  TCNT1=0x0000; 

  /* Configure the prescaler for 1:1024, giving us a 
   * timeout of 4.09 seconds.
   */
//  TCCR1B=0x03;

  /* Enable the timer overlow interrupt. */
//  TIMSK1=0x01;
}

void loop() {
  if(Serial.available()) {
    processSyncMessage();
  }

  ButtonUpState = digitalRead(ButtonUpPin);
  
  if (ButtonUpState != LastButtonUpState) {
    if (ButtonUpState == LOW) {
      NewBrightness += NewBrightness < 0xF0 ? 0x10 : 0;
      BrightnessChanged = true;
    }
  }
  LastButtonUpState = ButtonUpState;

  ButtonDownState = digitalRead(ButtonDownPin);
  if (ButtonDownState != LastButtonDownState) {
    if (ButtonDownState == LOW) {
      NewBrightness -= NewBrightness > 0 ? 0x10 : 0;
      BrightnessChanged = true;
    }
  }
  LastButtonDownState = ButtonDownState;

  if (NewBrightness != Brightness) {
    if (NewBrightness > Brightness) {
      Brightness++;
    } else if (NewBrightness < Brightness) {
      Brightness--;
    }
    LEDS.setBrightness(Brightness);
    LEDS.show();
  }

  if (BrightnessChanged) {
    BrightnessChanged = false;

    memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
    for (uint8_t i=0; i<= NewBrightness/0x10; i++) {
      leds[15 - i].r = 255; 
    }
    LEDS.show();
    delay(1000);
  }

  // fetch TimeZone from EEPROM
  while (!eeprom_is_ready());
  TimeZone = eeprom_read_word((uint16_t*)TimeZoneAddr);

  now = DateTime(RTC.now().unixtime() + TimeZone);

  if (now.second() != last_now.second()) {
    showCircleClock(now);
  }
  
  last_now = now;
//  enterSleep();
  delay(250);
}
