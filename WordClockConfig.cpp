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

extern int brightness = 200;

/**
  Night mode can be confiureg so that the clock is less bright and consumes less power.
  */
extern bool isNightModeEnabled = true;
extern bool isNightModeActive = false;
extern int nightModeStartHour = 23;
extern int nightModeEndtHour = 6;
extern int nightModeBrightness = 50;



//TimeChangeRule aEDT = {"AEDT", First, Sun, Oct, 2, 660};    //UTC + 11 hours
//TimeChangeRule aEST = {"AEST", First, Sun, Apr, 3, 600};    //UTC + 10 hours
// TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
// TimeChangeRule CET = {"CET ", Last, Sun, Oct, 30, 60};       //Central European Standard Time
//TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
//TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
//TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  //Eastern Daylight Time = UTC - 4 hours
//TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   //Eastern Standard Time = UTC - 5 hours
//TimeChangeRule usCDT = {"CDT", Second, dowSunday, Mar, 2, -300};
//TimeChangeRule usCST = {"CST", First, dowSunday, Nov, 2, -360};
//TimeChangeRule usMDT = {"MDT", Second, dowSunday, Mar, 2, -360};
//TimeChangeRule usMST = {"MST", First, dowSunday, Nov, 2, -420};
//TimeChangeRule usPDT = {"PDT", Second, dowSunday, Mar, 2, -420};
//TimeChangeRule usPST = {"PST", First, dowSunday, Nov, 2, -480};

//Timezone ausET(aEDT, aEST); //Australia Eastern Time Zone (Sydney, Melbourne)
// Timezone CE(CEST, CET); //Central European Time (Frankfurt, Paris)
//Timezone UK(BST, GMT); //United Kingdom (London, Belfast)
//Timezone usET(usEDT, usEST); //US Eastern Time Zone (New York, Detroit)
//Timezone usCT(usCDT, usCST); //US Central Time Zone (Chicago, Houston)
//Timezone usMT(usMDT, usMST); //US Mountain Time Zone (Denver, Salt Lake City)
//Timezone usAZ(usMST, usMST); //Arizona is US Mountain Time Zone but does not use DST
//Timezone usPT(usPDT, usPST); //US Pacific Time Zone (Las Vegas, Los Angeles)

// Timezone timezones[] = {CE};

// byte timezone = 0;
