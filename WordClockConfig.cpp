#include "WordClockConfig.h"

const byte ES[] =      {0, 0, 2};
const byte IST[] =     {0, 3, 3};
const byte FUENF[] =   {0, 7, 4};
const byte ZEHN[] =    {1, 0, 4};
const byte ZWANZIG[] = {1, 4, 7};
const byte VIERTEL[] = {2, 4, 7};
const byte NACH[] =    {3, 2, 4};
const byte VOR[] =     {3, 6, 3};
const byte HALB[] =    {4, 0, 4};
const byte UHR[] =     {9, 8, 3};

const byte HOUR[][3] = {
  //EIN       EINS       ZWEI       DREI       VIER       FUENF
  {5, 2, 3}, {5, 2, 4}, {5, 0, 4}, {6, 1, 4}, {7, 7, 4}, {6, 7, 4},
  //SECHS     SIEBEN     ACHT       NEUN       ZEHN       ELF        ZWOELF
  {9, 1, 5}, {5, 5, 6}, {8, 1, 4}, {7, 3, 4}, {8, 5, 4}, {7, 0, 3}, {4, 5, 5}
};

CRGB foreground = CRGB(0, 100, 100);
CRGB background = CRGB(5, 5, 5);

byte effect = 3;
byte hwVersion = 2;
bool showEsIst = false;
