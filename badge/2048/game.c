#include <stdlib.h>

#include "../../core/systick/systick.h"

#include "game.h"
#include "render.h"

enum {
  SPAWN_FALSE,
  SPAWN_TRUE
};

enum {
  END_FLAG_NO,
  END_FLAG_LOST,
  END_FLAG_NOW
};

uint8_t spawn_flag, end_flag;

void spawn_number(uint16_t grid[GRID_SIZE][GRID_SIZE]) {
  uint8_t x_grid, y_grid, value;

  for(;;) {
    x_grid = rand() % 4;
    y_grid = rand() % 4;
    
    if(grid[x_grid][y_grid] == 0) {
      value = rand() % 100;
      if(value < 90)
	grid[x_grid][y_grid] = 1;
      else
	grid[x_grid][y_grid] = 2;
      break;
    }
  }

  spawn_flag = SPAWN_FALSE;
}

void handle_input_event() {
  if(badge_event_current_input_state() & 
     (BADGE_EVENT_KEY_BTN_A | 
      BADGE_EVENT_KEY_BTN_B)) {
    end_flag = END_FLAG_NOW;
    return;
  }
}

void new_2048(void) {
  badge_framebuffer fb;
  badge_event_t ev;
  uint16_t grid[GRID_SIZE][GRID_SIZE] = { { 0 } };

  srand(systickGetTicks());

  badge_framebuffer_clear(&fb);
  show_frame(&fb);

  spawn_flag = SPAWN_TRUE;
  end_flag = END_FLAG_NO;

  do {
    if(spawn_flag)
      spawn_number(grid);
    show_grid(&fb, grid);

    ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT)
      handle_input_event();
  } while(end_flag == 0);
  
  if(end_flag == END_FLAG_NOW)
    return;
}

