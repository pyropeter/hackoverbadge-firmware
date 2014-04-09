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

enum {
  MOVE_UP,
  MOVE_DOWN,
  MOVE_RIGHT,
  MOVE_LEFT
};

enum {
  MOVE_IMPOSSIBLE,
  MOVE_POSSIBLE
};

enum {
  COULDNT_MOVE,
  COULD_MOVE
};

uint8_t spawn_flag, end_flag;

uint8_t move_possible(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
  uint8_t value, neighbor;
  
  for(uint8_t x = 0; x < 4; x++) {
    for(uint8_t y = 0; y < 4; y++) {
      value = grid[x][y];

      if(value == 0)
	return MOVE_POSSIBLE;

      if(y > 0) {
	neighbor = grid[x][y - 1];
	if(value == neighbor)
	  return MOVE_POSSIBLE;
      }

      if(y < 3) {
	neighbor = grid[x][y + 1];
	if(value == neighbor)
	  return MOVE_POSSIBLE;
      }

      if(x > 0) {
	neighbor = grid[x - 1][y];
	if(value == neighbor)
	  return MOVE_POSSIBLE;
      }

      if(x < 3) {
	neighbor = grid[x][y + 1];
	if(value == neighbor)
	  return MOVE_POSSIBLE;
      }
    }
  }

  end_flag = END_FLAG_LOST;
  return MOVE_IMPOSSIBLE;
}

void move(uint8_t grid[GRID_SIZE][GRID_SIZE], uint8_t direction) {
  int8_t tmp, move_flag;
  uint8_t merged[GRID_SIZE] = { 0 };

  move_flag = COULDNT_MOVE;

  switch(direction) {
  case MOVE_UP:
    for(int8_t x = 0; x < 4; x++) {
      for(int8_t y = 1; y < 4; y++) {
	tmp = y;
	
	while(tmp > 0) {
	  if(grid[x][tmp] > 0 && grid[x][tmp - 1] == 0) {
	    grid[x][tmp - 1] = grid[x][tmp];
	    grid[x][tmp--] = 0;
	    move_flag = COULD_MOVE;
	  } else if(grid[x][tmp] > 0 && grid[x][tmp - 1] == grid[x][tmp] && 
		    !((merged[tmp - 1] >> (3 - x)) & 1)) {
	    grid[x][tmp - 1] += 1;
	    grid[x][tmp] = 0;
	    merged[tmp - 1] |= 1 << (3 - x);
	    tmp = 0;
	    move_flag = COULD_MOVE;
	  } else {
	    tmp = 0;
	  }
	}
      }
    }
    break;
  case MOVE_DOWN:
    for(int8_t x = 0; x < 4; x++) {
      for(int8_t y = GRID_SIZE - 2; y >= 0; y--) {
	tmp = y;
	
	while(tmp < 3) {
	  if(grid[x][tmp] > 0 && grid[x][tmp + 1] == 0) {
	    grid[x][tmp + 1] = grid[x][tmp];
	    grid[x][tmp++] = 0;
	    move_flag = COULD_MOVE;
	  } else if(grid[x][tmp] > 0 && grid[x][tmp + 1] == grid[x][tmp] && 
		    !((merged[tmp + 1] >> (3 - x)) & 1)) {
	    grid[x][tmp + 1] += 1;
	    grid[x][tmp] = 0;
	    merged[tmp + 1] |= 1 << (3 - x);
	    tmp = 3;
	    move_flag = COULD_MOVE;
	  } else {
	    tmp = 3;
	  }
	}
      }
    }
    break;
  case MOVE_RIGHT:
    for(int8_t y = 0; y < 4; y++) {
      for(int8_t x = GRID_SIZE - 2; x >= 0; x--) {
	tmp = x;
	
	while(tmp < 3) {
	  if(grid[tmp][y] > 0 && grid[tmp + 1][y] == 0) {
	    grid[tmp + 1][y] = grid[tmp][y];
	    grid[tmp++][y] = 0;
	    move_flag = COULD_MOVE;
	  } else if(grid[tmp][y] > 0 && grid[tmp + 1][y] == grid[tmp][y] &&
		    !((merged[y] >> (3 - (tmp + 1))) & 1)) {
	    grid[tmp + 1][y] += 1;
	    grid[tmp][y] = 0;
	    merged[y] |= 1 << (3 - (tmp + 1));
	    tmp = 3;
	    move_flag = COULD_MOVE;
	  } else {
	    tmp = 3;
	  }
	}
      }
    }
    break;
  case MOVE_LEFT:
    for(int8_t y = 0; y < 4; y++) {
      for(int8_t x = 1; x < 4; x++) {
	tmp = x;
	
	while(tmp > 0) {
	  if(grid[tmp][y] > 0 && grid[tmp - 1][y] == 0) {
	    grid[tmp - 1][y] = grid[tmp][y];
	    grid[tmp--][y] = 0;
	    move_flag = COULD_MOVE;
	  } else if(grid[tmp][y] > 0 && grid[tmp - 1][y] == grid[tmp][y] &&
		    !((merged[y] >> (3 - (tmp - 1))) & 1)) {
	    grid[tmp - 1][y] += 1;
	    grid[tmp][y] = 0;
	    merged[y] |= 1 << (3 - (tmp - 1));
	    tmp = 0;
	    move_flag = COULD_MOVE;
	  } else {
	    tmp = 0;
	  }
	}
      }
    }
    break;
  }

  if(move_flag)
    spawn_flag = SPAWN_TRUE;
}

void spawn_number(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
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

void handle_input_event(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
  switch(badge_event_current_input_state()) {
  case BADGE_EVENT_KEY_BTN_A:
  case BADGE_EVENT_KEY_BTN_B: end_flag = END_FLAG_NOW; break;
  case BADGE_EVENT_KEY_UP: move(grid, MOVE_UP); break;
  case BADGE_EVENT_KEY_DOWN: move(grid, MOVE_DOWN); break;
  case BADGE_EVENT_KEY_RIGHT: move(grid, MOVE_RIGHT); break;
  case BADGE_EVENT_KEY_LEFT: move(grid, MOVE_LEFT); break;
  }
}

void new_2048(void) {
  badge_framebuffer fb;
  badge_event_t ev;
  uint8_t grid[GRID_SIZE][GRID_SIZE] = { { 0 } };

  srand(systickGetTicks());

  badge_framebuffer_clear(&fb);
  show_frame(&fb);

  spawn_flag = SPAWN_TRUE;
  end_flag = END_FLAG_NO;

  do {
    if(spawn_flag)
      spawn_number(grid);
    show_grid(&fb, grid);

    if(!move_possible(grid)) 
      break;

    ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_USER_INPUT)
      handle_input_event(grid);
  } while(end_flag == 0);
  
  if(end_flag == END_FLAG_NOW)
    return;
  else if(end_flag == END_FLAG_LOST) {
    //irgendwas wie einmal "LOST" aufflackern lassen oder so

    return;
  }
}

