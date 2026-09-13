#pragma once

#include <stdint.h>

struct WindowResizeData
{
    uint32_t width;
    uint32_t height;
};
typedef struct WindowResizeData WindowResizeData;

struct WindowMoveData
{
    uint32_t x;
    uint32_t y;
};
typedef struct WindowMoveData WindowMoveData;
