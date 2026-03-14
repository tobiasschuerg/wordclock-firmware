#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <FastLED.h>
#include "LedUtils.h"

#define SNAKE_UP    0
#define SNAKE_RIGHT 1
#define SNAKE_DOWN  2
#define SNAKE_LEFT  3

#define SNAKE_COLS 11
#define SNAKE_ROWS 10
#define SNAKE_MAX_LENGTH 30

struct SnakeSegment {
    byte row;
    byte col;
};

extern bool snakeGameActive;

void snakeStart();
void snakeStop();
void snakeSetDirection(byte dir);
bool snakeUpdate(unsigned long currentMillis);

#endif // SNAKE_GAME_H
