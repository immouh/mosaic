#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
  game g;
  if (argc == 1) {
    g = game_default();
  } else {
    g = game_load(argv[1]);
  }
  bool quit = false;

  while (!game_won(g) && !quit) {
    game_print(g);
    printf("? [h for help]\n");

    for (int i = 0; i < g->height; i++)
      for (int j = 0; j < g->width; j++)
        if (game_get_status(g, i, j) == ERROR)
          printf("Square (%d %d) : Error\n", i, j);

    char c;
    int r = scanf(" %c", &c);
    if (r) {
      if (c == 'h') {
        printf(
            "- press w <i> <j> to set square (i,j) white\n"
            "- press b <i> <j> to set square (i,j) black\n"
            "- press e <i> <j> to set square (i,j) empty\n"
            "- press y to redo\n"
            "- press z to undo\n"
            "- press r to restart\n"
            "- press q to quit\n"
            "- press s <filename> to save\n");
      }

      else if (c == 'r') {
        game_restart(g);
      } else if (c == 'q') {
        quit = true;

      } else if (c == 'z') {
        game_undo(g);
      } else if (c == 'y') {
        game_redo(g);
      } else if (c == 's') {
        char filename[1024];
        if (!(scanf("%s", filename) == 1)) exit(EXIT_FAILURE);
        game_save(g, filename);
      }

      else if (c == 'w' || c == 'b' || c == 'e') {
        int i;
        int j;
        r = scanf(" %d %d", &i, &j);
        if (r && i < g->height && i >= 0 && j < g->width && j >= 0) {
          if (c == 'w')
            game_play_move(g, i, j, WHITE);
          else if (c == 'b')
            game_play_move(g, i, j, BLACK);
          else
            game_play_move(g, i, j, EMPTY);
        }
      }
    }
  }
  game_print(g);
  if (quit)
    printf("Shame\n");
  else
    printf("Congratulation\n");
  return EXIT_SUCCESS;
}
