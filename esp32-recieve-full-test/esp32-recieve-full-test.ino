//#define FASTLED_ALLOW_INTERRUPTS 0

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

#define rxPin 2
RCSwitch mySwitch = RCSwitch();

#define KEEP_SHOWING_FOR_MILLIS 60000 // after receiving radio input, animate the portal for KEEP_SHOWING_FOR_MILLIS milliseconds
long lastRadioInput = millis();

uint8_t blueHue = 130;
uint8_t orangeHue = 20;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// setup function. runs once, then loop() runs forever
void setup() {
  Serial.begin(115200);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(rxPin, INPUT_PULLUP);
  mySwitch.enableReceive(rxPin);  // Receiver on interrupt 0 => that is arduino pin #2 (EXCEPT on ESP32, we put the actual PIN number here)
}

// main loop, which executes forever
void loop() {
  demo();
  
//  // handle incoming radio rx
//  if (mySwitch.available()) {
//    Serial.print("Radio received a value: ");
//    long receivedValue = mySwitch.getReceivedValue();
//    Serial.println(receivedValue);
//    
//    lastRadioInput = millis();  
//    mySwitch.resetAvailable();
//  }
//
//  // animate the portal if we recently recieved radio input
//  if (millis() < lastRadioInput + KEEP_SHOWING_FOR_MILLIS){
//    animatePortal(orangeHue);
//  }
//  else { // otherwise, turn off the LEDs
//    fill_solid( leds, NUM_LEDS, CRGB::Black);
//  }
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
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
