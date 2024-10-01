#ifndef __GAME_AUX__
#include "game_aux.h"

#include "game_struct.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#endif
void game_print(cgame g) {
  char* white_char[] = {"□", "🄋", "➀", "➁", "➂", "➃",
                        "➄", "➅",    "➆", "➇", "➈"};
  char* black_char[] = {"■", "🄌", "➊", "➋", "➌", "➍",
                        "➎", "➏",    "➐", "➑", "➒"};
  char empty_char[] = {' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  int w = g->width;
  int h = g->height;
  printf("   ");
  for (int i = 0; i < w; i++) {
    printf("%d ", i);
  }
  printf("\n   ");
  for (int i = 0; i < w; i++) {
    printf("--");
  }
  printf("\n");
  for (int i = 0; i < h; i++) {
    printf("%d |", i);
    for (int j = 0; j < w; j++) {
      color cl = game_get_color(g, i, j);
      constraint ct =
          game_get_constraint(g, i, j); /* Gets the value of the constraint to
                                           directly get the character needed*/
      if (cl == WHITE) {
        printf("%s ", white_char[ct + 1]);
      } else if (cl == BLACK) {
        printf("%s ", black_char[ct + 1]);
      } else {
        printf("%c ", empty_char[ct + 1]);
      }
    }
    printf("|\n");
  }
  printf("   ");
  for (int i = 0; i < w; i++) {
    printf("--");
  }
  printf("\n");
}

game game_default(void) {
  constraint uc = UNCONSTRAINED;
  constraint constraints[] = {0,  uc, uc, 3,  uc, uc, 5,  uc, uc, uc, uc, uc, 4,
                              uc, 1,  6,  uc, 6,  3,  uc, uc, uc, uc, uc, uc};
  game g = game_new(constraints, NULL);
  return g;
}

game game_default_solution(void) {
  constraint uc = UNCONSTRAINED;
  color w = WHITE;
  color b = BLACK;
  constraint constraints[] = {0,  uc, uc, 3,  uc, uc, 5,  uc, uc, uc, uc, uc, 4,
                              uc, 1,  6,  uc, 6,  3,  uc, uc, uc, uc, uc, uc};
  color colors[] = {w, w, b, w, w, w, w, b, w, b, b, b, b,
                    w, w, b, b, w, w, w, b, b, b, b, w};
  game g = game_new(constraints, colors);
  return g;
}
