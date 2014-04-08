#ifndef INCLUDED_BADGE2013_2048_GAME_H
#define INCLUDED_BADGE2013_2048_GAME_H

#include "../ui/display.h"
#include "../ui/event.h"

#define GRID_SIZE 4

extern uint8_t spawn_flag;
extern uint8_t end_flag;

void move(uint16_t grid[GRID_SIZE][GRID_SIZE], uint8_t direction);
void spawn_number(uint16_t grid[GRID_SIZE][GRID_SIZE]);
void handle_input_event();
void new_2048(void);

#endif
