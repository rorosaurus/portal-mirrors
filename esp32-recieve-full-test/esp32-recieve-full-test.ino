#include "FastLED.h"
#include <RCSwitch.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    13 // this is the pin that is connected to LED data IN
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    90
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 192 // out of 255 total
#define FRAMES_PER_SECOND 120
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define rxPin 34
RCSwitch mySwitch = RCSwitch();

#define KEEP_SHOWING_FOR_MILLIS 10000 // after receiving radio input, animate the portal for KEEP_SHOWING_FOR_MILLIS milliseconds
long lastRadioTimestamp = millis();
long lastRadioValue;

uint8_t blueHue = 130;
uint8_t orangeHue = 20;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns[10];
//{ pride, pacifica, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

TaskHandle_t ledTask;

// setup function. runs once, then loop() runs forever
void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
      ledTaskCode, /* Function to implement the task */
      "ledTask", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &ledTask,  /* Task handle. */
      0); /* Core where the task should run */

  pinMode(rxPin, INPUT_PULLUP);
  mySwitch.enableReceive(rxPin);
}

// main loop, which executes forever
void loop() {
  // handle incoming radio rx
  if (mySwitch.available()) {
    Serial.print("Radio received a value: ");
    lastRadioValue = mySwitch.getReceivedValue();
    Serial.println(lastRadioValue);
    
    lastRadioTimestamp = millis();  
    mySwitch.resetAvailable();
  }
}

void ledTaskCode( void * parameter) {
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // infinite loop
  for(;;) {
//    demo();
//    pride();
    
    if (millis() < lastRadioTimestamp + KEEP_SHOWING_FOR_MILLIS){ // animate the portal if we recently recieved radio input
      if (lastRadioValue == 5393) animatePortal(orangeHue);
      else if (lastRadioValue == 5396) animatePortal(blueHue);
      else pride();
    }
    else { // otherwise, turn off the LEDs
      fill_solid( leds, NUM_LEDS, CRGB::Black);
    }
    
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND);
  
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  }
}

// ====================
// animation functions
// ====================
uint8_t numberOfPulses = 16;
int phaseMultiplier = (360/numberOfPulses)*256; // maybe make this less exact? slightly random for each pulse, so the pattern isn't perfectly repeating?

void animatePortal(uint8_t hue) {

  fadeToBlackBy( leds, NUM_LEDS, 18); // out of 256 = dim by x/256th each frame. too much dimming will reveal the reds in orange.
  
  portalJuggle(1, hue);
  portalJuggle(4, hue);
  portalJuggle(5, hue);
}

void portalJuggle(uint8_t bpm, uint8_t hue) {
  for( int i = 0; i < numberOfPulses; i++) { // create numberOfPulses travelling pulses
    uint8_t pos = beatsin16( bpm /* BPM, 1-7 seems ok */, 0, NUM_LEDS-1, 0, i*phaseMultiplier); // calculate the position to set the color

    // calculate an offset for the hue, based on the height of each pixel
    uint8_t hueOffset = 0;
    if (hue == blueHue) hueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 30, 0);
    if (hue == orangeHue) hueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 0, 30);

    uint8_t valueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 96, 0); // slightly dim the upper pixels
    
    // assign the calculated color to the correct pixel
    leds[pos] |= CHSV(hue+hueOffset, 255, 255-valueOffset);
  }
}

void demo() {
  static bool oldOrange = false;
  static bool oldBlue = false;
  
  if (millis() % 20000 > 10000){
    if(oldOrange == false){
      oldOrange = true;
      oldBlue = false;
      fill_solid( leds, NUM_LEDS, CRGB::Black);
    }
    animatePortal(orangeHue);
  }
  else {
    if(oldBlue == false){
      oldBlue = true;
      oldOrange = false;
      fill_solid( leds, NUM_LEDS, CRGB::Black);
    }
    animatePortal(blueHue);
  }
}
