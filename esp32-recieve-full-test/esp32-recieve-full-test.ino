//#define FASTLED_ALLOW_INTERRUPTS 0

#include "FastLED.h"
#include <RCSwitch.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    13 // this is the pin that is connected to LED data IN
#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define NUM_LEDS    90
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 10 // 25/255 = 1/10th brightness
#define FRAMES_PER_SECOND 120
#define CHANGE_PATTERN_SECONDS 10 // automatically change to the next pattern after this many seconds

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

RCSwitch mySwitch = RCSwitch();

#define KEEP_SHOWING_FOR_MILLIS 2000
long lastHeartbeat = millis();

// setup function. runs once, then loop() runs forever
void setup() {
  Serial.begin(115200);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(2, INPUT_PULLUP);
  mySwitch.enableReceive(2);  // Receiver on interrupt 0 => that is pin #2 EXCEPT on ESP32, put the actual PIN number here
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint32_t lastPatternChange = millis(); // keep track of the last time we changed patterns
uint8_t gHue = 200; // rotating "base color" used by many of the patterns

// main loop, which executes forever
void loop() {
//  Serial.println("test");
  if (mySwitch.available()) {
    Serial.print("Received: ");
    Serial.println( mySwitch.getReceivedValue() );
    lastHeartbeat = millis();  
    mySwitch.resetAvailable();
  }

  if (millis() < lastHeartbeat + KEEP_SHOWING_FOR_MILLIS){
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
  }
  else {
    fill_solid( leds, NUM_LEDS, CRGB::Black);
  }
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  //EVERY_N_MILLISECONDS(200) { fill_solid( leds, NUM_LEDS, CHSV( gHue, 200, 255)); } // make sure we don't get too far from blue
  EVERY_N_SECONDS(CHANGE_PATTERN_SECONDS) { patternTimer(); } // change patterns periodically
  
}

void patternTimer() {
  if (lastPatternChange + (CHANGE_PATTERN_SECONDS * 1000) <= millis()) {
    nextPattern();
  }
}

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
  lastPatternChange = millis();
}

// ====================
// animation functions
// ====================

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() {
  
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 25);
//  for (int i=0; i < NUM_LEDS; i++){
//    leds[i] -= CHSV( gHue , 200, 255);
//  }
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(32), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
