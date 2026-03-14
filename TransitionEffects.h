#ifndef TRANSITION_EFFECTS_H
#define TRANSITION_EFFECTS_H

#include <FastLED.h>

void transitionFade(CRGB on, CRGB off);
void transitionTypewriter(CRGB on, CRGB off);
void transitionRollDown(CRGB on, CRGB off);
void transitionWave(CRGB on, CRGB off);
void transitionSlide(CRGB on, CRGB off);
void transitionPulse(CRGB on, CRGB off);

#endif // TRANSITION_EFFECTS_H
