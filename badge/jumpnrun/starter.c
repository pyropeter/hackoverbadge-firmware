#include "jumpnrun.h"
#include "../ui/menu.h"

#include <drivers/fatfs/ff.h>

enum {
  LEVELFILE_MAX        = 12,
  LEVELDESCRIPTION_MAX = 14,
  MENU_BUFLEN          = LEVELDESCRIPTION_MAX + 1 + LEVELFILE_MAX + 1
};

static uint8_t jumpnrun_pick_level_from_fd(char *buf, size_t *first_visible, size_t *selected, FIL *fd) {
  unsigned levelcount = 0;

  {
    char buf[MENU_BUFLEN];
    while(f_gets(buf, MENU_BUFLEN, fd)) {
      ++levelcount;
    }
  }

  if(FR_OK != f_lseek(fd, 0)) {
    return JUMPNRUN_ERROR;
  }

  char menu_buf[levelcount][MENU_BUFLEN];
  char const *menu_index[levelcount];
  char const *fnames[levelcount];
  unsigned i;

  for(i = 0; i < levelcount && f_gets(menu_buf[i], MENU_BUFLEN, fd); ++i) {
    menu_index[i] = menu_buf[i];
    char *p;
    for(p = menu_buf[i]; *p && *p != '|'; ++p)
      ;
    if(*p) {
      *p++ = '\0';
    }
    fnames[i] = p;
  }

  *selected = badge_menu(menu_index, i, first_visible, *selected);

  strcpy(buf, fnames[*selected]);
  return 0;
}

static uint8_t jumpnrun_pick_level(char *buf, size_t *first_visible, size_t *selected) {
  FIL fd;

  if(FR_OK != f_open(&fd, "levels.lst", FA_OPEN_EXISTING | FA_READ)) {
    return JUMPNRUN_ERROR;
  }

  uint8_t err = jumpnrun_pick_level_from_fd(buf, first_visible, selected, &fd);

  f_close(&fd);

  return err;
}

void jumpnrun_play(void) {
  char buf[LEVELFILE_MAX + 1];
  size_t first_visible = 0;
  size_t selected = 0;

  while(0 == jumpnrun_pick_level(buf, &first_visible, &selected)) {
    jumpnrun_play_level(buf);
  }
}