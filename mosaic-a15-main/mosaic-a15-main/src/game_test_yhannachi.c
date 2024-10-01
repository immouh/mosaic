#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#define ASSERT(expr)                                                          \
  do {                                                                        \
    if ((expr) == 0) {                                                        \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, \
              #expr);                                                         \
      abort();                                                                \
    }                                                                         \
  } while (0)

bool test_dummy() { return true; }

bool test_game_save() {
  game g1 = game_default();
  game g2 = game_default_solution();
  ASSERT(g1);
  ASSERT(g2);

  game_save(g1, "f_default");
  game_save(g2, "f_solution");

  game gd = game_load("f_default");
  game gf = game_load("f_solution");
  ASSERT(gf);
  ASSERT(gd);
  ASSERT(game_equal(g1, gd));
  ASSERT(game_equal(g2, gf));
  game_delete(g1);
  game_delete(g2);
  game_delete(gd);
  game_delete(gf);
  return true;
}

bool test_game_equal() {
  // Checking equality between a game and its copy.
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_copy(g1);
  ASSERT(g2);
  ASSERT(game_equal(g1, g2));
  game_delete(g1);
  game_delete(g2);

  // Checking inequality between games with different sizes:
  game g = game_new_empty();
  ASSERT(g);
  game g_modified_height =
      game_new_empty_ext(game_nb_rows(g) + 1, game_nb_cols(g), false, FULL);
  ASSERT(g_modified_height);
  game g_modified_width =
      game_new_empty_ext(game_nb_rows(g), game_nb_cols(g) - 1, false, FULL);
  ASSERT(g_modified_width);
  ASSERT(!game_equal(g, g_modified_height) && !game_equal(g, g_modified_width));

  game_delete(g);
  game_delete(g_modified_height);
  game_delete(g_modified_width);

  // Checking inequality between games with differents neighbourhood
  game game_tab[4];
  for (int i = 0; i < 4; i++) {
    game_tab[i] = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, i);
    ASSERT(game_tab[i]);
  }
  ASSERT(!game_equal(game_tab[0], game_tab[1]) &&
         !game_equal(game_tab[0], game_tab[2]) &&
         !game_equal(game_tab[0], game_tab[3]));

  for (int i = 0; i < 4; i++) {
    game_delete(game_tab[i]);
  }

  // Checking inequality between games with different wrapping status.
  for (int i = 0; i < 2; i++) {
    game_tab[i] = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, i, FULL);
    ASSERT(game_tab[i]);
  }
  ASSERT(!game_equal(game_tab[0], game_tab[1]));

  for (int i = 0; i < 2; i++) {
    game_delete(game_tab[i]);
  }
  // Checking inequality between games with different square's colors or
  // constraints.
  game gc = game_new_empty();
  ASSERT(gc);

  game g_different_color = game_copy(gc);
  ASSERT(g_different_color);
  game_set_color(g_different_color, 0, 1, BLACK);

  game g_different_constraint = game_copy(gc);
  ASSERT(g_different_constraint);
  game_set_constraint(g_different_constraint, 0, 1, 3);

  ASSERT(!game_equal(gc, g_different_color) &&
         !game_equal(gc, g_different_constraint));

  game_delete(gc);
  game_delete(g_different_color);
  game_delete(g_different_constraint);

  return true;
}

bool test_game_delete() {
  // Just calling the function to see if there is no memory leak.
  game g = game_default();
  ASSERT(g);
  game_delete(g);
  return true;
}

bool test_game_set_color() {
  game g1 = game_new_empty();
  ASSERT(g1);
  game g2 = game_new_empty();
  ASSERT(g2);
  game g3 = game_new_empty();
  ASSERT(g3);

  game_set_color(g1, 1, 0, BLACK);
  ASSERT(game_get_color(g1, 1, 0) == BLACK);
  ASSERT(game_get_color(g1, 0, 1) == EMPTY);
  // check if there is no index inversion:
  game_set_color(g3, 0, 1, BLACK);
  ASSERT(game_get_color(g3, 1, 0) == EMPTY);
  ASSERT(game_get_color(g3, 0, 1) == BLACK);
  // check if the colors are properly applied by checking if the games are not
  // equal:
  ASSERT(!game_equal(g1, g2));
  ASSERT(!game_equal(g1, g3));
  // freeing up the games:
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);

  return true;
}

bool test_game_set_constraint() {
  game g1 = game_new_empty();
  ASSERT(g1);
  game g2 = game_new_empty();
  ASSERT(g2);
  game g3 = game_new_empty();
  ASSERT(g3);

  game_set_constraint(g1, 0, 1, 3);
  ASSERT(game_get_constraint(g1, 0, 1) == 3);
  ASSERT(game_get_constraint(g1, 1, 0) == -1);
  // check if there is no index inversion
  game_set_constraint(g3, 1, 0, 3);
  ASSERT(game_get_constraint(g3, 0, 1) == -1);
  ASSERT(game_get_constraint(g3, 1, 0) == 3);
  // check if the constraints are properly applied by checking if the games are
  // not equal:
  ASSERT(!game_equal(g1, g2));
  ASSERT(!game_equal(g1, g3));
  // freeing up the games:
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);

  return true;
}

bool test_game_get_color() {
  game g1 = game_new_empty();
  ASSERT(g1);
  // Setting up the three colors in three different squares:
  game_set_color(g1, 0, 0, BLACK);
  game_set_color(g1, 0, 1, EMPTY);
  game_set_color(g1, 0, 2, WHITE);

  // Checking if the colors are correctly applied:
  ASSERT(game_get_color(g1, 0, 0) == BLACK);
  ASSERT(game_get_color(g1, 0, 1) == EMPTY);
  ASSERT(game_get_color(g1, 0, 2) == WHITE);

  game_delete(g1);

  return true;
}

bool test_game_get_constraint() {
  game g1 = game_new_empty();
  ASSERT(g1);
  // Check for the unconstrained square status (EMPTY_GAME-->All squares are
  // unconstrained):
  ASSERT(game_get_constraint(g1, 0, 0) == -1);

  // Check the case where a square is constrained:
  game_set_constraint(g1, 0, 0, 1);
  ASSERT(game_get_constraint(g1, 0, 0) == 1);

  game_delete(g1);
  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
  if (argc == 1) usage(argc, argv);

  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;
  if (strcmp("dummy", argv[1]) == 0) {
    ok = test_dummy();
  } else if (strcmp("game_equal", argv[1]) == 0) {
    ok = test_game_equal();
  } else if (strcmp("game_delete", argv[1]) == 0) {
    ok = test_game_delete();
  } else if (strcmp("game_set_color", argv[1]) == 0) {
    ok = test_game_set_color();
  } else if (strcmp("game_set_constraint", argv[1]) == 0) {
    ok = test_game_set_constraint();
  } else if (strcmp("game_get_color", argv[1]) == 0) {
    ok = test_game_get_color();
  } else if (strcmp("game_get_constraint", argv[1]) == 0) {
    ok = test_game_get_constraint();
  } else if (strcmp("game_save", argv[1]) == 0) {
    ok = test_game_save();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
