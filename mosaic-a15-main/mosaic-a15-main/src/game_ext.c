#ifndef _GAME_EXT_H
#define _GAME_EXT_H
#include "game_ext.h"

#include <stdio.h>
#include <stdlib.h>

#include "game_struct.h"
#endif

game game_new_ext(uint nb_rows, uint nb_cols, constraint *constraints,
                  color *colors, bool wrapping, neighbourhood neigh) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, neigh);
  for (int i = 0; i < nb_cols * nb_rows; i++) {
    g->constraints[i] = constraints[i];
    if (colors != NULL) {
      g->colors[i] = colors[i];
    } else {
      g->colors[i] = EMPTY;
    }
  }
  return g;
}

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping,
                        neighbourhood neigh) {
  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  g->height = nb_rows;
  g->width = nb_cols;
  g->constraints = malloc(nb_rows * nb_cols * sizeof(constraint));
  g->colors = malloc(nb_rows * nb_cols * sizeof(color));
  g->neighbourhood = neigh;
  g->wrapping = wrapping;
  g->prv_moves = queue_new();
  g->undone_moves = queue_new();
  if (g->constraints == NULL || g->colors == NULL) {
    fprintf(stderr, "Memory allocation failed");
    game_delete(g);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < nb_cols * nb_rows; i++) {
    g->constraints[i] = UNCONSTRAINED;
    g->colors[i] = EMPTY;
  }
  return g;
}

uint game_nb_rows(cgame g) { return g->height; }

uint game_nb_cols(cgame g) { return g->width; }

bool game_is_wrapping(cgame g) { return g->wrapping; }

neighbourhood game_get_neighbourhood(cgame g) { return g->neighbourhood; }

void game_undo(game g) {
  if (queue_is_empty(g->prv_moves)) {
    return;
  }
  /* Takes the array defined in game_play_move from the queue to undo the last
  move, and saves what color the square was in the other queue*/
  uint *t = queue_pop_head(g->prv_moves);
  color c = game_get_color(g, *t, *(t + 1));
  game_set_color(g, *t, *(t + 1), *(t + 2));
  *(t + 2) = c;
  queue_push_head(g->undone_moves, t);
}

void game_redo(game g) {
  if (queue_is_empty(g->undone_moves)) {
    return;
  }
  /* Same principle as game_undo, cancel the last call of game_undo*/
  uint *t = queue_pop_head(g->undone_moves);
  color c = game_get_color(g, *t, *(t + 1));
  game_set_color(g, *t, *(t + 1), *(t + 2));
  *(t + 2) = c;
  queue_push_head(g->prv_moves, t);
}
