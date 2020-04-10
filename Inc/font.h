#pragma once

#define START_LINE 12
#define CHAR_COUNT_Y 27


void TV_CursorOn();
void TV_CursorOff();
void TV_GoToXY(int x, int y);
void TV_PrintChar(char ch);
int TV_PrintLine(const char *text);
int TV_PrintText(const char *text);