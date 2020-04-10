#pragma once

#define TV_USART_BAUDRATE 10000

typedef enum {
    TV_START_CMD = 204,
    TV_CLEAR_SCREEN_CMD  = 200,
    TV_CURSOR_ON_CMD = 201,
    TV_CURSOR_OFF_CMD = 202,
    TV_SET_CURSOR_CMD = 203,
} TV_Command;

typedef enum {
    TV_ERROR  = 255,
    TV_OK = 0
} TV_Status;
