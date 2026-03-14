#include "SnakeGame.h"
#include <Arduino.h>

extern CRGB leds[];

bool snakeGameActive = false;

static SnakeSegment body[SNAKE_MAX_LENGTH];
static byte snakeLength;
static byte currentDir;
static byte pendingDir;
static byte foodRow;
static byte foodCol;
static unsigned long lastTick;
static const unsigned long TICK_MS = 200;

static bool flashing;
static unsigned long flashStart;
static const unsigned long FLASH_MS = 300;

static void spawnFood() {
    byte r, c;
    bool occupied;
    do {
        r = random(SNAKE_ROWS);
        c = random(SNAKE_COLS);
        occupied = false;
        for (byte i = 0; i < snakeLength; i++) {
            if (body[i].row == r && body[i].col == c) {
                occupied = true;
                break;
            }
        }
    } while (occupied);
    foodRow = r;
    foodCol = c;
}

static void render() {
    fillLeds(CRGB::Black);
    for (int i = SNAKE_ROWS * SNAKE_COLS; i < SNAKE_ROWS * SNAKE_COLS + 4; i++) {
        leds[i] = CRGB::Black;
    }
    setLeds(foodRow, foodCol, CRGB::Red, 1, false);
    for (byte i = 0; i < snakeLength; i++) {
        setLeds(body[i].row, body[i].col, CRGB::Green, 1, false);
    }
}

void snakeStart() {
    snakeLength = 3;
    currentDir = SNAKE_RIGHT;
    pendingDir = SNAKE_RIGHT;
    body[0] = {5, 5};
    body[1] = {5, 4};
    body[2] = {5, 3};
    flashing = false;
    snakeGameActive = true;
    lastTick = millis();
    spawnFood();
}

void snakeStop() {
    snakeGameActive = false;
}

void snakeSetDirection(byte dir) {
    if ((dir == SNAKE_UP && currentDir == SNAKE_DOWN) ||
        (dir == SNAKE_DOWN && currentDir == SNAKE_UP) ||
        (dir == SNAKE_LEFT && currentDir == SNAKE_RIGHT) ||
        (dir == SNAKE_RIGHT && currentDir == SNAKE_LEFT)) {
        return;
    }
    pendingDir = dir;
}

bool snakeUpdate(unsigned long currentMillis) {
    if (!snakeGameActive) return false;

    if (flashing) {
        if (currentMillis - flashStart < FLASH_MS) {
            fillLeds(CRGB::Black);
            for (int i = SNAKE_ROWS * SNAKE_COLS; i < SNAKE_ROWS * SNAKE_COLS + 4; i++) {
                leds[i] = CRGB::Black;
            }
            // Flash only the snake body red (low power draw)
            for (byte i = 0; i < snakeLength; i++) {
                setLeds(body[i].row, body[i].col, CRGB(80, 0, 0), 1, false);
            }
            return true;
        } else {
            snakeGameActive = false;
            return false;
        }
    }

    if (currentMillis - lastTick < TICK_MS) {
        render();
        return true;
    }
    lastTick = currentMillis;

    currentDir = pendingDir;

    byte newRow = body[0].row;
    byte newCol = body[0].col;
    switch (currentDir) {
        case SNAKE_UP:    newRow = (newRow == 0) ? SNAKE_ROWS - 1 : newRow - 1; break;
        case SNAKE_DOWN:  newRow = (newRow + 1) % SNAKE_ROWS; break;
        case SNAKE_LEFT:  newCol = (newCol == 0) ? SNAKE_COLS - 1 : newCol - 1; break;
        case SNAKE_RIGHT: newCol = (newCol + 1) % SNAKE_COLS; break;
    }

    for (byte i = 0; i < snakeLength; i++) {
        if (body[i].row == newRow && body[i].col == newCol) {
            flashing = true;
            flashStart = currentMillis;
            return true;
        }
    }

    bool ate = (newRow == foodRow && newCol == foodCol);

    if (!ate) {
        for (byte i = snakeLength - 1; i > 0; i--) {
            body[i] = body[i - 1];
        }
    } else {
        if (snakeLength < SNAKE_MAX_LENGTH) {
            for (byte i = snakeLength; i > 0; i--) {
                body[i] = body[i - 1];
            }
            snakeLength++;
        } else {
            for (byte i = snakeLength - 1; i > 0; i--) {
                body[i] = body[i - 1];
            }
        }
        spawnFood();
    }

    body[0] = {newRow, newCol};

    render();
    return true;
}
