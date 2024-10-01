#define __USE_GNU
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

constraint uc = UNCONSTRAINED;

#define ASSERT(expr)                                                          \
  do {                                                                        \
    if ((expr) == 0) {                                                        \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, \
              #expr);                                                         \
      abort();                                                                \
    }                                                                         \
  } while (0)

bool test_dummy() { return true; }

bool test_new(void) {
  // Creating an empty game by calling the function game_new_empty
  game g1 = game_new_empty();
  ASSERT(g1);

  // Manually creating an empty game:
  unsigned int size = DEFAULT_SIZE * DEFAULT_SIZE;
  constraint constraints[size];
  color colors[size];
  for (int i = 0; i < size; i++) {
    constraints[i] = uc;
    colors[i] = EMPTY;
  }
  game g2 = game_new(constraints, colors);
  ASSERT(g2);

  // Check if both are equal:
  ASSERT(game_equal(g1, g2));
  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_new_empty(void) {
  game g = game_new_empty();
  ASSERT(g);
  // Checking cell by cell if their color is empty and they are unconstrained:
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      ASSERT(game_get_color(g, i, j) == EMPTY);
      ASSERT(game_get_constraint(g, i, j) == uc);
    }
  }
  // freeing memory allocated for games
  game_delete(g);
  return true;
}

bool test_copy(void) {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_copy(g1);
  ASSERT(g2);
  ASSERT(game_equal(g1, g2));

  // Checking options

  game g3 = game_new_empty_ext(6, 7, true, FULL_EXCLUDE);
  ASSERT(g3);
  game g4 = game_copy(g3);
  ASSERT(game_equal(g3, g4));

  // freeing memory allocated for games

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_print() {
  // Just calling the function itself to see if it's working.
  game g = game_default();
  ASSERT(g);
  game_print(g);
  // freeing memory allocated for games
  game_delete(g);
  return true;
}

bool test_default() {
  game g1 = game_default();
  ASSERT(g1);
  // Manually creating game_default
  constraint constraints[] = {0,  uc, uc, 3,  uc, uc, 5,  uc, uc, uc, uc, uc, 4,
                              uc, 1,  6,  uc, 6,  3,  uc, uc, uc, uc, uc, uc};
  game g2 = game_new(constraints, NULL);
  ASSERT(g2);
  ASSERT(game_equal(g1, g2));
  // freeing memory allocated for games

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_default_solution() {
  game g1 = game_default_solution();
  game g2 = game_default();
  ASSERT(g1);
  ASSERT(g2);

  // Check for constraints
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      ASSERT(game_get_constraint(g1, i, j) == game_get_constraint(g2, i, j));
    }
  }

  // Verify if it's actually the solution
  ASSERT(game_won(g1));
  // Freeing memory allocated for games

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_new_ext() {
  game g1 = game_default();
  ASSERT(g1);
  constraint constraints[] = {0,  uc, uc, 3,  uc, uc, 5,  uc, uc, uc, uc, uc, 4,
                              uc, 1,  6,  uc, 6,  3,  uc, uc, uc, uc, uc, uc};
  game g2 = game_new_ext(5, 5, constraints, NULL, false, FULL);
  ASSERT(g2);
  game g3 = game_new_ext(5, 5, constraints, NULL, true, ORTHO);
  ASSERT(g3);

  ASSERT(game_equal(g1, g2));  // checking if the default game created with the
                               // function match the actual default game
  ASSERT(!game_equal(g1, g3));

  // Checking if the  options are correctly initialised:
  ASSERT(game_get_neighbourhood(g2) == FULL);
  ASSERT(game_get_neighbourhood(g3) == ORTHO);
  ASSERT(game_is_wrapping(g3));
  ASSERT(!game_is_wrapping(g2));

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}

bool test_new_empty_ext() {
  game g1 = game_new_empty();
  ASSERT(g1);
  game g2 = game_new_empty_ext(5, 5, false, FULL);
  ASSERT(g2);
  game g3 = game_new_empty_ext(6, 6, false, FULL);
  ASSERT(g3);
  game g4 = game_new_empty_ext(6, 6, true, ORTHO);
  ASSERT(g4);

  ASSERT(game_equal(g1, g2));
  ASSERT(!game_equal(g1, g3));
  ASSERT(!game_equal(g1, g4));

  // Checking if the  options are correctly initialised:
  ASSERT(game_is_wrapping(g4));
  ASSERT(game_get_neighbourhood(g3) == FULL);

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_undo() {
  game g1 = game_default();
  ASSERT(g1);
  game_play_move(g1, 0, 0, WHITE);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);

  game_play_move(g1, 0, 0, BLACK);
  ASSERT(game_get_color(g1, 0, 0) == BLACK);

  game_undo(g1);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);
  game_undo(g1);
  ASSERT(game_get_color(g1, 0, 0) == EMPTY);
  game_redo(g1);
  // freeing memory allocated for games

  game_delete(g1);
  return true;
}

bool test_redo() {
  game g1 = game_default();
  ASSERT(g1);
  game_play_move(g1, 0, 0, WHITE);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);

  game_play_move(g1, 0, 0, BLACK);
  ASSERT(game_get_color(g1, 0, 0) == BLACK);

  game_undo(g1);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);
  game_redo(g1);
  ASSERT(game_get_color(g1, 0, 0) == BLACK);
  game_undo(g1);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);
  game_undo(g1);
  ASSERT(game_get_color(g1, 0, 0) == EMPTY);
  game_redo(g1);
  ASSERT(game_get_color(g1, 0, 0) == WHITE);
  game_redo(g1);
  ASSERT(game_get_color(g1, 0, 0) == BLACK);
  // freeing memory allocated for games

  game_delete(g1);
  return true;
}

bool test_load() {
  game g1 = game_load("default.txt");
  game g2 = game_load("solutions.txt");
  game g3 = game_default();
  game g4 = game_default_solution();
  ASSERT(g1);
  ASSERT(g2);
  ASSERT(g3);
  ASSERT(g4);
  ASSERT(game_equal(g1, g3));
  ASSERT(game_equal(g2, g4));
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
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
  if (strcmp("dummy", argv[1]) == 0)
    ok = test_dummy();
  else if (strcmp("new_empty", argv[1]) == 0)
    ok = test_new_empty();
  else if (strcmp("new", argv[1]) == 0)
    ok = test_new();
  else if (strcmp("copy", argv[1]) == 0)
    ok = test_copy();
  else if (strcmp("print", argv[1]) == 0)
    ok = test_print();
  else if (strcmp("default", argv[1]) == 0)
    ok = test_default();
  else if (strcmp("default_solution", argv[1]) == 0)
    ok = test_default_solution();
  else if (strcmp("new_ext", argv[1]) == 0)
    ok = test_new_ext();
  else if (strcmp("new_empty_ext", argv[1]) == 0)
    ok = test_new_empty_ext();
  else if (strcmp("undo", argv[1]) == 0)
    ok = test_undo();
  else if (strcmp("redo", argv[1]) == 0)
    ok = test_redo();
  else if (strcmp("load", argv[1]) == 0)
    ok = test_load();
  else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if (ok) {
    fprintf(stderr, "Test\"%s\" finished : SUCCESS\n", argv[1]);
    exit(EXIT_SUCCESS);
  } else {
    fprintf(stderr, "Test\"%s\" finished : FAILURE\n", argv[1]);
    exit(EXIT_FAILURE);
  }
}
