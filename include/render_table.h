#ifndef RENDER_TABLE_H
#define RENDER_TABLE_H

#include "consts.h"

#define RENDER_TABLE_SIZE CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT

typedef enum {
    ON,
    OFF,
} PixelState;

typedef struct {
    PixelState render_table[RENDER_TABLE_SIZE];
} RenderTable;

void rt_init(RenderTable* render_table);
bool rt_is_pixel_on(RenderTable* render_table, uint8_t x, uint8_t y);
void rt_change_pixel_state(RenderTable* render_table, uint8_t x, uint8_t y, PixelState new_state);
void rt_clear(RenderTable* render_table);

#endif
