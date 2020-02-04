//#define FASTLED_ALLOW_INTERRUPTS 0

#include "FastLED.h"
#include <RCSwitch.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    13 // this is the pin that is connected to LED data IN
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    90
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 192 // 25/255 = 1/10th brightness
#define FRAMES_PER_SECOND 120
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

RCSwitch mySwitch = RCSwitch();

#define KEEP_SHOWING_FOR_MILLIS 60000
long lastHeartbeat = millis();

uint8_t blueHue = 130;  // old 212
uint8_t orangeHue = 20; // old 89
//
//uint8_t purpleHue = ;
//uint8_t yellowHue = ;

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

// main loop, which executes forever
void loop() {
  if (mySwitch.available()) {
    // TODO: read in portal color value
    Serial.print("Received: ");
    Serial.println( mySwitch.getReceivedValue() );
    lastHeartbeat = millis();  
    mySwitch.resetAvailable();
  }

//  if (millis() < lastHeartbeat + KEEP_SHOWING_FOR_MILLIS){
  if (millis() % 20000 > 10000){
    animatePortal(orangeHue);
  }
  else {
    animatePortal(blueHue);
//    fill_solid( leds, NUM_LEDS, CRGB::Black);
  }
//  juggle();
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
}

// ====================
// animation functions
// ====================
uint8_t numberOfPulses = 16;
int phaseMultiplier = (360/numberOfPulses)*256;

void animatePortal(uint8_t hue) {
  // 16 colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 8); // 8/256 = dim by 1/32 each frame
  for( int i = 0; i < numberOfPulses; i++) { // create numberOfPulses travelling pulses
    uint8_t pos = beatsin16( 7, 0, NUM_LEDS-1, 0, i*phaseMultiplier); // calculate the position to set the color

    // calculate an offset for the hue, based on the height of each pixel
    uint8_t hueOffset = 0;
    if (hue == blueHue) hueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 30, 0);
    if (hue == orangeHue) hueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 0, 30);

    uint8_t valueOffset = map(abs(pos-(NUM_LEDS/2)), 0, NUM_LEDS/2, 96, 0);
    // assign the calculated color to the correct pixel
    leds[pos] |= CHSV(hue+hueOffset, 255, 255-valueOffset);
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(25, 255, 255);
  }
}
