#ifndef _GAME_H
#define _GAME_H
#include "game.h"

#include <sys/types.h>

#include "game_ext.h"
#include "game_struct.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#endif  // __GAME_H__

game game_new(constraint *constraints, color *colors) {
  game g = game_new_empty();
  for (int i = 0; i < g->height * g->width; i++) {
    g->constraints[i] = constraints[i];
    if (colors != NULL) {
      g->colors[i] = colors[i];
    } else {
      g->colors[i] = EMPTY;
    }
  }
  return g;
}

game game_new_empty(void) {
  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  uint size = DEFAULT_SIZE;
  g->height = size;
  g->width = size;
  g->constraints = malloc(size * size * sizeof(constraint));
  g->colors = malloc(size * size * sizeof(color));
  if (g->constraints == NULL || g->colors == NULL) {
    fprintf(stderr, "Memory allocation failed");
    game_delete(g);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < size * size; i++) {
    g->constraints[i] = UNCONSTRAINED;
    g->colors[i] = EMPTY;
  }
  g->neighbourhood = FULL;
  g->wrapping = false;
  g->prv_moves = queue_new();
  g->undone_moves = queue_new();

  return g;
}

game game_copy(cgame g) {
  game g2 =
      game_new_empty_ext(g->height, g->width, g->wrapping, g->neighbourhood);
  for (int i = 0; i < g->height * g->width; i++) {
    g2->constraints[i] = g->constraints[i];
    g2->colors[i] = g->colors[i];
  }

  return g2;
}

bool game_equal(cgame g1, cgame g2) {
  if (g1->height != g2->height || g1->width != g2->width ||
      (game_is_wrapping(g1)) != (game_is_wrapping(g2)) ||
      (game_get_neighbourhood(g1)) != game_get_neighbourhood(g2)) {
    return false;
  }
  for (int i = 0; i < g1->height; i++) {
    for (int j = 0; j < g1->width; j++) {
      if ((game_get_constraint(g1, i, j) != game_get_constraint(g2, i, j)) ||
          (game_get_color(g1, i, j) != game_get_color(g2, i, j))) {
        return false;
      }
    }
  }
  return true;
}

void game_delete(game g) {
  free(g->colors);
  free(g->constraints);
  queue_free_full(g->prv_moves, free);
  queue_free_full(g->undone_moves, free);
  free(g);
}

void game_set_constraint(game g, uint i, uint j, constraint n) {
  // modified the constraints table using the row-major order to access to the
  // case.
  g->constraints[(g->width * i) + j] = n;
}

void game_set_color(game g, uint i, uint j, color c) {
  // modified the colors table using the row-major order to access to the case.
  g->colors[(g->width * i) + j] = c;
}

constraint game_get_constraint(cgame g, uint i, uint j) {
  return (g->constraints[(g->width * i) + j]);
}

color game_get_color(cgame g, uint i, uint j) {
  return (g->colors[(g->width * i) + j]);
}

bool game_get_next_square(cgame g, uint i, uint j, direction dir, uint *pi_next,
                          uint *pj_next) {
  // first we'll work on the y coordinates:
  if (dir == UP || dir == UP_LEFT || dir == UP_RIGHT) {
    *pi_next = i - 1;
    if (g->wrapping && i == 0) {
      *pi_next = g->height - 1;
    }
  } else if (dir == DOWN_LEFT || dir == DOWN_RIGHT || dir == DOWN) {
    *pi_next = i + 1;
    if (g->wrapping && i == g->height - 1) {
      *pi_next = 0;
    }
  } else {
    *pi_next = i;
  }
  // on the x:
  if (dir == LEFT || dir == UP_LEFT || dir == DOWN_LEFT) {
    *pj_next = j - 1;
    if (g->wrapping && j == 0) {
      *pj_next = g->width - 1;
    }
  } else if (dir == DOWN_RIGHT || dir == RIGHT || dir == UP_RIGHT) {
    *pj_next = j + 1;
    if (g->wrapping && j == g->width - 1) {
      *pj_next = 0;
    }
  } else {
    *pj_next = j;
  }
  // checking if the net_pos is in the correct field:
  if (*pi_next >= g->height || *pi_next < 0 || *pj_next >= g->width ||
      *pj_next < 0) {
    return false;
  }
  return true;
}

status game_get_status(cgame g, uint i, uint j) {
  // Special case: the square has no constraints:
  if (game_get_constraint(g, i, j) == UNCONSTRAINED) {
    if (game_nb_neighbors(g, i, j, EMPTY) == 0) {
      return SATISFIED;
    }
    return UNSATISFIED;
  }
  // ERROR CASE:
  if ((game_nb_neighbors(g, i, j, BLACK) >
       game_get_constraint(
           g, i,
           j)) ||  // either there is more black squares than the constraint.
      (game_nb_neighbors(g, i, j, EMPTY) <
       game_get_constraint(g, i, j) -
           game_nb_neighbors(
               g, i, j, BLACK))) {  // or there is too much white squares in
                                    // order for the condition to be met.
    return ERROR;
  }
  // SATISFIED CASE:
  else if ((game_nb_neighbors(g, i, j, BLACK) ==
            game_get_constraint(g, i, j)) &&
           (game_nb_neighbors(g, i, j, EMPTY) == 0)) {
    return SATISFIED;
  } else {
    return UNSATISFIED;
  }
}

int game_nb_neighbors(cgame g, uint i, uint j, color c) {
  // actually need to deal with the four neihbours possibilities
  int count = 0;
  uint i2;
  uint j2;
  if (g->neighbourhood == FULL) {
    direction directions_FULL[] = {
        UP_LEFT, UP, UP_RIGHT, LEFT, HERE, RIGHT, DOWN_LEFT, DOWN, DOWN_RIGHT};
    for (int k = 0; k < 9; k++) {
      if (game_get_next_square(g, i, j, directions_FULL[k], &i2, &j2) &&
          game_get_color(g, i2, j2) == c) {
        count++;
      }
    }
  } else if (g->neighbourhood == ORTHO) {
    direction directions_ORTHO[] = {
        UP, LEFT, HERE, RIGHT, DOWN,
    };
    for (int k = 0; k < 5; k++) {
      if (game_get_next_square(g, i, j, directions_ORTHO[k], &i2, &j2) &&
          game_get_color(g, i2, j2) == c) {
        count++;
      }
    }
  } else if (g->neighbourhood == ORTHO_EXCLUDE) {
    direction directions_ORTHO_EXCLUDE[] = {
        UP,
        LEFT,
        RIGHT,
        DOWN,
    };
    for (int k = 0; k < 4; k++) {
      if (game_get_next_square(g, i, j, directions_ORTHO_EXCLUDE[k], &i2,
                               &j2) &&
          game_get_color(g, i2, j2) == c) {
        count++;
      }
    }
  } else {
    direction directions_FULL_EXCLUDE[] = {
        UP_LEFT, UP, UP_RIGHT, LEFT, RIGHT, DOWN_LEFT, DOWN, DOWN_RIGHT};
    for (int k = 0; k < 8; k++) {
      if (game_get_next_square(g, i, j, directions_FULL_EXCLUDE[k], &i2, &j2) &&
          game_get_color(g, i2, j2) == c) {
        count++;
      }
    }
  }
  return count;
}

void game_play_move(game g, uint i, uint j, color c) {
  color prev_color = game_get_color(g, i, j);
  /*The list of played moves for game_undo and game_redo is done with arrays of
  3 elements, respectively containing the 2 positions of the move, and the
  overwritten color */
  uint *tab = malloc(3 * sizeof(uint));
  *(tab) = i;
  *(tab + 1) = j;
  *(tab + 2) = prev_color;
  queue_push_head(g->prv_moves, tab);
  game_set_color(g, i, j, c);
  queue_clear(g->undone_moves);
}

bool game_won(cgame g) {
  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      if (game_get_status(g, i, j) != SATISFIED) {
        return false;
      }
    }
  }
  return true;
}

void game_restart(game g) {
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      game_set_color(g, i, j, EMPTY);
    }
  }
  queue_clear_full(g->prv_moves, free);
  queue_clear_full(g->undone_moves, free);
}
