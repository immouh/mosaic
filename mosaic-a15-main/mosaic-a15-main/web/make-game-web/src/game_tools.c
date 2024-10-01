/**
 * @file game_tools.c
 * @copyright University of Bordeaux. All rights reserved, 2023.
 **/

#include "game_tools.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                               LOAD & SAVE                                  */
/* ************************************************************************** */


game game_load(char* filename) {
  int rows = 0;
  int columns = 0;
  int wrapping = 0;
  int neigh = 0;
  FILE* fp = fopen(filename, "r");
  if (!(fscanf(fp, "%d %d %d %d", &rows, &columns, &wrapping, &neigh) ==
        4))
    exit(EXIT_FAILURE);
  int constraints[rows * columns];
  color colors[rows * columns];
  char next_color;
  char next_constraint;
  fgetc(fp);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (!(fscanf(fp, "%c %c", &next_constraint, &next_color) == 2))
        exit(EXIT_FAILURE);
      switch (next_color) {
        case 'e':
          colors[i * columns + j] = EMPTY;
          break;
        case 'b':
          colors[i * columns + j] = BLACK;
          break;
        case 'w':
          colors[i * columns + j] = WHITE;
          break;
      }
      switch (next_constraint) {
        case '-':
          constraints[i * columns + j] = -1;
          break;

        default:
          constraints[i * columns + j] = atoi(&next_constraint);
          break;
      }
    }
    fgetc(fp);
  }
  game g =
      game_new_ext(rows, columns, constraints, colors, wrapping, neigh);
  fclose(fp);
  return g;
}

void game_save(cgame g, char* filename) {
  FILE* f = fopen(filename, "wb");
  int main_parameters[4] = {game_nb_rows(g), game_nb_cols(g),
                            game_is_wrapping(g), game_get_neighbourhood(g)};
  fprintf(f, "%d", main_parameters[0]);
  for (int i = 1; i < 4; i++) {
    fprintf(f, " %d", main_parameters[i]);
  }

  char col[3] = {'e', 'w', 'b'};
  for (int i = 0; i < game_nb_rows(g); i++) {
    fprintf(f, "\n");
    for (int j = 0; j < game_nb_cols(g); j++) {
      int cons = game_get_constraint(g, i, j);
      int col_index = game_get_color(g, i, j);
      if (cons != -1) {
        fprintf(f, "%c%c", (cons + '0'), col[col_index]);
      } else {
        fprintf(f, "-%c", col[col_index]);
      }
    }
  }
  fclose(f);
}

int pow2(int n) {
  // Math.h libraries seems to not be usable for moodle, so this will have to do
  int result = 1;
  for (int i = 0; i < n; i++) {
    result = result * 2;
  }
  return result;
}
void fillsquares(game g, int i, int j, color c, int index_squares[],
                 int* solved_squares) {
  unsigned int i2;
  unsigned int j2;
  int nb_cols = g->nb_cols;
  int array_length = 0;
  direction directions[9];

  if (g->neigh == FULL) {
    directions[0] = UP_LEFT;
    directions[1] = UP;
    directions[2] = UP_RIGHT;
    directions[3] = LEFT;
    directions[4] = HERE;
    directions[5] = RIGHT;
    directions[6] = DOWN_LEFT;
    directions[7] = DOWN;
    directions[8] = DOWN_RIGHT;
    array_length = 9;
  }
  if (g->neigh == ORTHO) {
    directions[0] = UP;
    directions[1] = LEFT;
    directions[2] = HERE;
    directions[3] = RIGHT;
    directions[4] = DOWN;
    array_length = 5;
  }

  if (g->neigh == FULL_EXCLUDE) {
    directions[0] = UP_LEFT;
    directions[1] = UP;
    directions[2] = UP_RIGHT;
    directions[3] = LEFT;
    directions[4] = RIGHT;
    directions[5] = DOWN_LEFT;
    directions[6] = DOWN;
    directions[7] = DOWN_RIGHT;
    array_length = 8;
  }

  if (g->neigh == ORTHO_EXCLUDE) {
    directions[0] = UP;
    directions[1] = LEFT;
    directions[2] = RIGHT;
    directions[3] = DOWN;
    array_length = 4;
  }

  for (int k = 0; k < array_length; k++) {
    if (game_get_next_square(g, i, j, directions[k], &i2, &j2) &&
        game_get_color(g, i2, j2) == EMPTY) {
      game_set_color(g, i2, j2, c);
      index_squares[*solved_squares] = i2 * nb_cols + j2;
      *solved_squares += 1;
    }
  }
}

int presolve_game(game g, int index_squares[]) {
  int nb_cols = g->nb_cols;
  int nb_rows = g->nb_rows;
  int solved_squares = 0;
  int test_flag = -1;
  while (solved_squares != test_flag) {
    test_flag = solved_squares;
    for (int i = 0; i < nb_rows; i++) {
      for (int j = 0; j < nb_cols; j++) {
        constraint constraint = game_get_constraint(g, i, j);
        if (constraint != UNCONSTRAINED &&
            game_get_status(g, i, j) == UNSATISFIED) {
          if (constraint - game_nb_neighbors(g, i, j, BLACK) == 0)
            fillsquares(g, i, j, WHITE, index_squares, &solved_squares);
          else if (game_nb_neighbors(g, i, j, EMPTY) ==
                   constraint - game_nb_neighbors(g, i, j, BLACK))
            fillsquares(g, i, j, BLACK, index_squares, &solved_squares);
        }
      }
    }
  }
  return solved_squares;
}
void fill_game(game g, short int word[], uint squares[], uint len) {
  for (int k = 0; k < len; k++) {
    int i = squares[k] / (g->nb_cols);
    int j = squares[k] % (g->nb_cols);
    game_set_color(g, i, j, word[k]);
  }
}

void genWord(int num, int len, short int* word) {
  for (int i = len - 1; i >= 0; i--) {
    word[i] = (num % 2) + 1;
    num = num >> 1;
  }
}

bool game_solve(game g) {
  game_restart(g);
  uint len_words = g->nb_rows * g->nb_cols;
  int index_squares[len_words];
  int squares_solved = presolve_game(g, index_squares);
  if (game_won(g)) {
    return true;
  }
  len_words = len_words - squares_solved;
  short int word[len_words];
  uint index_unsolved_squares[len_words];
  uint index = 0;
  for (int i = 0; i < len_words + squares_solved; i++) {
    bool is_solved = false;
    for (int j = 0; j < squares_solved; j++) {
      if (index_squares[j] == i) is_solved = true;
    }
    if (!is_solved) {
      index_unsolved_squares[index] = i;
      index++;
    }
  }
  int numword = pow2(len_words);
  for (int i = 0; i < numword; i++) {
    genWord(i, len_words, word);
    fill_game(g, word, index_unsolved_squares, len_words);
    if (game_won(g)) {
      return true;
    }
  }
  return false;
}

uint game_nb_solutions(cgame g) {
  game g2 = game_copy(g);
  uint len_words = g->nb_rows * g->nb_cols;
  int index_solved_squares[len_words];
  int nb_solved = presolve_game(g2, index_solved_squares);
  if (game_won(g2)) {
    game_delete(g2);
    return 1;
  }
  short int word[len_words];
  len_words -= nb_solved;
  uint index_unsolved_squares[len_words];

  int index = 0;

  // Construction du tableau des cases non trouvées
  for (int i = 0; i < len_words + nb_solved; i++) {
    bool is_solved = false;
    for (int j = 0; j < nb_solved; j++) {
      if (index_solved_squares[j] == i) is_solved = true;
    }
    if (!is_solved) {
      index_unsolved_squares[index] = i;
      index++;
    }
  }
  int numword = pow2(len_words);
  uint nb_solutions = 0;
  for (int i = 0; i < numword; i++) {
    genWord(i, len_words, word);
    fill_game(g2, word, index_unsolved_squares, len_words);
    if (game_won(g2)) {
      nb_solutions++;
    }
  }
  game_delete(g2);
  return nb_solutions;
}


/* ************************************************************************** */
/*                             Random Game Generator                          */
/* ************************************************************************** */

game game_random(uint nb_rows, uint nb_cols, bool wrapping, neighbourhood neigh,
                 bool with_solution, float black_rate, float constraint_rate)
{
  assert(black_rate >= 0.0f && black_rate <= 1.0f);
  assert(constraint_rate >= 0.0f && constraint_rate <= 1.0f);
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, neigh);
  assert(g);

  // fill the grid with random colors
  for (uint i = 0; i < nb_rows; i++)
  {
    for (uint j = 0; j < nb_cols; j++)
    {
      color c = (rand() < black_rate * (float)RAND_MAX) ? BLACK : WHITE;
      game_set_color(g, i, j, c);
    }
  }

  // fill the grid with actual constraint at random positions
  uint nb_squares = nb_rows * nb_cols;
  uint nb_constraints = constraint_rate * nb_squares;
  for (uint i = 0; i < nb_constraints; i++)
  {
    uint row = rand() % nb_rows;
    uint col = rand() % nb_cols;
    int nb_blacks = game_nb_neighbors(g, row, col, BLACK);
    game_set_constraint(g, row, col, nb_blacks);
  }

  // check solution
  if (!game_won(g))
  {
    game_delete(g);
    return NULL;
  }

  if (!with_solution)
    game_restart(g);
  return g;
}

/* ************************************************************************** */
