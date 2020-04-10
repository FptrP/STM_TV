#pragma once

#define BUFF_SIZE_Y 228
#define BUFF_SIZE_X 40
#define BUFF_SIZE_X_WITH_ZERO (BUFF_SIZE_X + 1)

struct TV_State {
    int lineCount;
    int syncOffset;
    uint8_t buffer[BUFF_SIZE_Y][BUFF_SIZE_X_WITH_ZERO];
};

extern struct TV_State TV;


void TV_Init();
void TV_Stop();
void TV_ClearScreen();  


