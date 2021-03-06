#include "badge_main_loop.h"

#include "jumpnrun/jumpnrun.h"
#include "ui/display.h"
#include "ui/event.h"
#include "ui/menu.h"
#include "ui/sprite.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LEVELS 1024

#define PATH_PREFIX "../badge/jumpnrun/levels/"

void badge_main_loop(void) {
  char menu_buf[MAX_LEVELS][32];
  FILE *fd = fopen("levels.txt", "r");

  int i;

  for(i = 0; i < MAX_LEVELS && fgets(menu_buf[i], sizeof(menu_buf[i]), fd); ++i) {
    menu_buf[i][strlen(menu_buf[i]) - 1] = '\0';
  }

  fclose(fd);

  char const *menu[i];
  for(int j = 0; j < i; ++j) {
    menu[j] = menu_buf[j];
  }

  uint8_t choice = 0;
  uint8_t first_visible = 0;

  for(;;) {
    choice = badge_menu(menu, ARRAY_SIZE(menu), &first_visible, choice);
    char lvname[256];
    sprintf(lvname, PATH_PREFIX "%s.lvl", menu[choice]);

    jumpnrun_play_level(lvname);
  }
}
