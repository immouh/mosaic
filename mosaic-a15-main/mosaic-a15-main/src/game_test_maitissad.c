#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

#define ASSERT(expr)                                                          \
  do {                                                                        \
    if (!(expr)) {                                                            \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, \
              #expr);                                                         \
      abort();                                                                \
    }                                                                         \
  } while (0)

bool test_dummy()

{
  return true;
}

bool test_game_nb_rows(void) {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_default();
  ASSERT(g2);
  game g3 = game_new_empty_ext(DEFAULT_SIZE + 1, DEFAULT_SIZE, false, FULL);
  ASSERT(g3);
  game g4 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, ORTHO);
  ASSERT(g4);
  ASSERT(game_nb_rows(g1) ==
         DEFAULT_SIZE);  // Checks if default game has DEFAULT_SIZE rows
  ASSERT(game_nb_rows(g3) ==
         DEFAULT_SIZE +
             1);  // Checks if custom game has one more row than default
  ASSERT(game_nb_rows(g1) ==
         game_nb_rows(g2));  // Compares row count of two default games
  ASSERT(game_nb_rows(g1) ==
         game_nb_rows(g4));  // Compares row count of default and ORTHO games
  ASSERT(
      game_nb_rows(g1) !=
      game_nb_rows(g3));  // Ensures different games report different row counts

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}
bool test_game_nb_cols(void) {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_default();
  ASSERT(g2);
  game g3 = game_new_empty_ext(
      DEFAULT_SIZE, DEFAULT_SIZE - 1, false,
      FULL);  // Create a game with one less column than the default size
  ASSERT(g3);
  game g4 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true,
                               ORTHO);  // Create a game with the default size
                                        // but with ORTHO neighborhood
  ASSERT(g4);
  // Validate that the number of columns is correctly reported by the function
  ASSERT(game_nb_cols(g1) ==
         DEFAULT_SIZE);  // Check default game's columns match the default size
  ASSERT(game_nb_cols(g3) ==
         DEFAULT_SIZE -
             1);  // Verify custom game's columns are one less than defaul
  ASSERT(game_nb_cols(g1) ==
         game_nb_cols(g2));  // Compare columns of two default games
  ASSERT(game_nb_cols(g1) ==
         game_nb_cols(
             g4));  // Default and ORTHO game should have same column count
  ASSERT(game_nb_cols(g1) !=
         game_nb_cols(
             g3));  // Ensure different games report different column counts

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_game_is_wrapping(void) {
  game g1 = game_default();
  ASSERT(g1);
  // Create a game with wrapping enabled
  game g2 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, true, FULL);
  ASSERT(g2);
  // Create a game without wrapping
  game g3 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, FULL);
  ASSERT(g3);

  ASSERT(game_is_wrapping(g1) ==
         false);  //  Confirm that default game do not have wrapping
  ASSERT(game_is_wrapping(g2) ==
         true);  //  Confirm that the custom have wrapping enabled
  ASSERT(game_is_wrapping(g2) !=
         game_is_wrapping(g1));  // Different wrapping status between g1 and g2
  ASSERT(
      game_is_wrapping(g1) ==
      game_is_wrapping(g3));  // g1 and g3 should have the same wrapping status

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  return true;
}
bool test_game_get_neighbourhood(void) {
  game g1 = game_default();  //(Default have a FULL Neighbourhood type).
  ASSERT(g1);

  // Create various games with different neighborhood settings
  game g2 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, FULL);
  ASSERT(g2);
  game g3 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, FULL_EXCLUDE);
  ASSERT(g3);
  game g4 = game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, ORTHO);
  ASSERT(g4);
  game g5 =
      game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, ORTHO_EXCLUDE);
  ASSERT(g5);

  ASSERT(game_get_neighbourhood(g1) ==
         game_get_neighbourhood(
             g2));  // g1 and g2 should have the same neighborhood type

  // Check if different neighborhood types are detected
  ASSERT(game_get_neighbourhood(g1) != game_get_neighbourhood(g3));
  ASSERT(game_get_neighbourhood(g1) != game_get_neighbourhood(g4));
  ASSERT(game_get_neighbourhood(g1) != game_get_neighbourhood(g5));
  ASSERT(game_get_neighbourhood(g3) != game_get_neighbourhood(g5));
  ASSERT(game_get_neighbourhood(g4) != game_get_neighbourhood(g5));

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  return true;
}

bool test_game_restart() {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_copy(g1);
  ASSERT(g2);

  // Modifying the state of the original game
  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      game_set_color(g1, i, j, BLACK);
    }
  }
  // Restart the game to its initial state
  game_restart(g1);
  // Check if the restarted game (g1) matches its original state (g2)
  ASSERT(game_equal(g1, g2));

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_won(void) {
  game g1 = game_default();
  ASSERT(g1);
  game g2 = game_default_solution();
  ASSERT(g2);
  game g3 = game_new_empty();
  ASSERT(g3);
  game g4 = game_default_solution();
  ASSERT(g4);
  game g5 = game_default_solution();
  ASSERT(g5);

  ASSERT(!game_won(
      g1));  // The default game without a solution (g1) should not be won
  ASSERT(game_won(g2));   // g2 should be in a winning state
  ASSERT(!game_won(g3));  // An empty game (g3) should not be in a winning state

  game_set_color(g4, 0, 0, BLACK);
  ASSERT(!game_won(g4));  // g4 should not be won after we modified a color

  game_set_constraint(g5, 0, 0, 2);
  ASSERT(!game_won(g5));  // g4 should not be won after we modified a constraint

  // Creation of a new game solution different from the game_default_solution.
  constraint uc = UNCONSTRAINED;
  color w = WHITE;
  color b = BLACK;
  constraint constraints[] = {1, uc, uc, uc, uc, 0, uc, 2, uc};
  color colors[] = {w, w, w, b, w, w, b, w, w};
  game other_solution = game_new_ext(3, 3, constraints, colors, false, FULL);
  ASSERT(game_won(
      other_solution));  // Check if the other solution is in a winning state

  // freeing memory allocated for games
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(other_solution);
  return true;
}

bool test_game_nb_neighbors() {
  // generating random values for the height and the width of the grid we'll be
  // testing.
  int nb_rows = rand() % 10;
  int nb_cols = rand() % 10;

  // Creating a game type array containing four 4 games, each of them with
  // different type of neighbourhood.
  game game_tab[4];
  for (int i = 0; i < 4; i++) {
    game_tab[i] = game_new_empty_ext(nb_rows, nb_cols, false, i);
    ASSERT(game_tab[i]);
  }

  // Creating a bi-dimensional array containing the nb_neighbours of a square
  // depending of its position and the neighbouthoud type of the game.
  int nb_neighbours[4][3] = {{4, 6, 9}, {3, 4, 5}, {3, 5, 8}, {2, 3, 4}};

  for (int k = 0; k < 4; k++) {
    for (uint i = 0; i < nb_rows; i++) {
      for (uint j = 0; j < nb_cols; j++) {
        color c = game_get_color(game_tab[k], i, j);
        if ((i == 0 || i == nb_rows - 1) &&
            (j == 0 || j == nb_cols - 1)) {  // The square is a corner.
          ASSERT(game_nb_neighbors(game_tab[k], i, j, c) ==
                 nb_neighbours[k][0]);

        } else if ((i == 0 || i == nb_rows - 1) ||
                   (j == 0 || j == nb_cols - 1)) {  // The square is and edge.
          ASSERT(game_nb_neighbors(game_tab[k], i, j, c) ==
                 nb_neighbours[k][1]);

        } else {  // The square neither a corner nor an edge.
          ASSERT(game_nb_neighbors(game_tab[k], i, j, c) ==
                 nb_neighbours[k][2]);
        }
      }
    }
  }

  // Deleting the games.
  for (int i = 0; i < 4; i++) {
    game_delete(game_tab[i]);
  }
  return true;
}

bool test_game_get_status(void) {
  game g = game_default();
  ASSERT(g);
  ASSERT(game_get_status(g, 0, 0) == UNSATISFIED);
  game_set_color(g, 0, 0, WHITE);
  game_set_color(g, 0, 1, WHITE);
  game_set_color(g, 1, 0, WHITE);
  game_set_color(g, 1, 1, WHITE);
  ASSERT(game_get_status(g, 0, 0) == SATISFIED);
  game_set_color(g, 0, 1, BLACK);
  ASSERT(game_get_status(g, 0, 0) == ERROR);
  game_delete(g);
  return true;
}

bool test_game_get_next_square() {
  game g = game_new_empty();
  uint i_next, j_next;
  uint* pi_next = &(i_next);
  uint* pj_next = &(j_next);
  ASSERT(
      game_get_next_square(g, 0, 0, DOWN_RIGHT, pi_next,
                           pj_next));  // Check if the function correctly
                                       // calculates the next square's position
  ASSERT(i_next == 1 && j_next == 1);  // DOWN_RIGHT from (0,0) should be (1,1)

  // Repeat for different directions and check the calculated positions
  ASSERT(game_get_next_square(g, 1, 1, UP_LEFT, pi_next, pj_next));
  ASSERT(i_next == 0 && j_next == 0);
  ASSERT(!game_get_next_square(g, 0, 0, UP_LEFT, pi_next,
                               pj_next));  // No square UP_LEFT of (0,0)
  ASSERT(game_get_next_square(g, 1, 1, UP, pi_next, pj_next));
  ASSERT(i_next == 0 && j_next == 1);
  ASSERT(game_get_next_square(g, 1, 1, DOWN, pi_next, pj_next));
  ASSERT(i_next == 2 && j_next == 1);
  ASSERT(game_get_next_square(g, 1, 1, RIGHT, pi_next, pj_next));
  ASSERT(i_next == 1 && j_next == 2);
  ASSERT(game_get_next_square(g, 1, 1, LEFT, pi_next, pj_next));
  ASSERT(i_next == 1 && j_next == 0);
  ASSERT(game_get_next_square(g, 1, 1, DOWN_LEFT, pi_next, pj_next));
  ASSERT(i_next == 2 && j_next == 0);
  ASSERT(game_get_next_square(g, 1, 1, UP_RIGHT, pi_next, pj_next));
  ASSERT(i_next == 0 && j_next == 2);

  // Test with a game that has wrapping enabled
  game g1 = game_new_empty_ext(5, 5, true, FULL);
  ASSERT(game_get_next_square(g1, 0, 0, UP_LEFT, pi_next, pj_next));
  ASSERT(i_next == 4 &&
         j_next == 4);  // UP_LEFT of (0,0) should wrap around to (4,4)
  ASSERT(game_get_next_square(g1, 4, 4, DOWN_RIGHT, pi_next, pj_next));
  ASSERT(i_next == 0 &&
         j_next == 0);  // DOWN_RIGHT of (4,4) should wrap around to (0,0)

  // freeing memory allocated for games
  game_delete(g);
  game_delete(g1);
  return true;
}

bool test_game_play_move() {
  game g1 = game_new_empty();
  ASSERT(g1);
  game_play_move(g1, 0, 0, WHITE);
  game_play_move(g1, 1, 0, BLACK);
  ASSERT(game_get_color(g1, 0, 0) ==
         WHITE);  // The square (0,0) should now be WHITE
  ASSERT(game_get_color(g1, 1, 0) ==
         BLACK);  // The square (1,0) should now be BLACK
  game_play_move(g1, 0, 0, EMPTY);

  ASSERT(game_get_color(g1, 0, 0) ==
         EMPTY);  // The square (0,0) should now be EMPTY

  // freeing memory allocated for games
  game_delete(g1);
  return true;
}

void usage(int argc, char* argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
  if (argc == 1) usage(argc, argv);

  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;

  if (strcmp("dummy", argv[1]) == 0) {
    ok = test_dummy();
  }

  else if (strcmp("game_restart", argv[1]) == 0) {
    ok = test_game_restart();
  } else if (strcmp("game_is_wrapping", argv[1]) == 0) {
    ok = test_game_is_wrapping();
  } else if (strcmp("game_nb_cols", argv[1]) == 0) {
    ok = test_game_nb_cols();
  } else if (strcmp("game_nb_rows", argv[1]) == 0) {
    ok = test_game_nb_rows();
  } else if (strcmp("game_won", argv[1]) == 0) {
    ok = test_game_won();
  } else if (strcmp("game_nb_neighbors", argv[1]) == 0) {
    ok = test_game_nb_neighbors();
  } else if (strcmp("game_get_status", argv[1]) == 0) {
    ok = test_game_get_status();
  } else if (strcmp("game_get_next_square", argv[1]) == 0) {
    ok = test_game_get_next_square();
  } else if (strcmp("game_play_move", argv[1]) == 0) {
    ok = test_game_play_move();
  } else if (strcmp("game_get_neighbourhood", argv[1]) == 0) {
    ok = test_game_get_neighbourhood();
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
