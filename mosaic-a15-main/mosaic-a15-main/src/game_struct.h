#ifndef __STRUCT_H__
#define __STRUCT_H__
#include "game_ext.h"
#include "queue.h"
struct game_s {
  int height;
  int width;
  constraint *constraints;
  color *colors;
  neighbourhood neighbourhood;
  bool wrapping;
  queue *prv_moves;
  queue *undone_moves;
};
#endif
