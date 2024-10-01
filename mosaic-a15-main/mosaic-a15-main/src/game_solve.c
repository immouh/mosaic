
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_struct.h"
#include "game_tools.h"
int main(int argc, char* argv[]) {
  if (argc <= 2) {
    printf("Syntax : ./game_solve <option> <input> [<output>]\n");
    printf("Possible inputs : -s, -c\n");
    return EXIT_FAILURE;
  }
  char* arg = argv[1];
  game g = game_load(argv[2]);
  if (strcmp(arg, "-s") == 0) {
    if (game_solve(g)) {
      if (argc == 4) {
        game_save(g, argv[3]);
      } else {
        game_print(g);
      }
      game_delete(g);
      return EXIT_SUCCESS;
    }
    game_delete(g);
    return EXIT_FAILURE;
  }
  if (strcmp(arg, "-c") == 0) {
    int n_solutions = game_nb_solutions(g);

    if (argc == 4) {
      FILE* fp = fopen(argv[3], "w");
      if (n_solutions != 0) {
        fprintf(fp, "%d", n_solutions);
      }
      fclose(fp);
    } else {
      printf("Number of solutions: %d\n", n_solutions);
    }
    game_delete(g);
    return EXIT_SUCCESS;
  } else {
    printf("Syntax : ./game_solve <option> <input> [<output>]\n");
    printf("Possible inputs : -s, -c");
    game_delete(g);
    return EXIT_FAILURE;
  }
}
