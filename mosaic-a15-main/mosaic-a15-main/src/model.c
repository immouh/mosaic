// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>  // required to load transparent texture from PNG
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>  // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Button.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

/* **************************************************************** */

// IMAGES
#define BACKGROUND "res/bg_game.png"

// BUTTONS
#define REDO "res/REDO.png"
#define UNDO "res/UNDO.png"
#define RESTART "res/RESTART.png"
#define START "res/START.png"
#define SOLVE "res/SOLVE.png"
#define LOAD "res/LOAD2.png"
#define SAVE "res/SAVE.png"
#define WIN "res/WIN.png"

// CELLS
#define WHITE_CELL "res/CASE_BLANCHE_2.png"
#define BLACK_CELL "res/CASE_NOIRE_2.png"
#define EMPTY_CELL "res/CASE_EMPTY_2.png"

// FONTS&&COLORS
#define FONT "res/SHADSER.TTF"
#define FONTSIZE 1000
#define WHITE \
  { 255, 255, 255, 0 }
#define BLACK {0, 0, 0, 0};
// BUTTONS
#define NB_BUTTONS_LG 4
#define NB_CELL_COLORS 3
#define NB_BUTTONS_L 2
// LOAD/SAVE
#define LOAD_SAVE "load_save.txt"

struct Env_t {
  SDL_Texture *background;

  // GRID DIM
  int cell_width, cell_height;
  int grid_width, grid_height;

  // GRID DRAWING
  SDL_Texture *digits_textures[3][10];
  SDL_Texture *cell_colors[3];
  // GAME
  game g;
  int game_nb_rows, game_nb_cols;

  // Bottom buttons
  Button *buttons_LG[NB_BUTTONS_LG];
  int button_x;
  int button_y;
  int button_width;
  int button_height;

  // LEFT SIDE BUTTONS
  Button *buttons_L[NB_BUTTONS_LG];
  int button_L_x;
  int button_L_y;
  int button_L_width;
  int button_L_height;

  // FONT/COLORS
  TTF_Font *font;
  SDL_Color satisfied;
  SDL_Color unsatisfied;
  SDL_Color error;
};

void compute_dims(Env *env, int w, int h) {
  env->game_nb_rows = game_nb_rows(env->g);
  env->game_nb_cols = game_nb_cols(env->g);
  env->grid_width = fmin(w, h) / 2;
  env->grid_height = fmin(w, h) / 2;
  int grid_x = (w / 2) - ((env->grid_width) / 2);
  int cell_w = ((env->grid_width / game_nb_cols(env->g)));
  int cell_h = env->grid_height / game_nb_rows(env->g);
  int cell_dim = fmin(cell_h, cell_w);
  env->cell_width = cell_dim;
  env->cell_height = cell_dim;

  // Bottom Buttons
  env->button_x = (env->cell_width);
  env->button_y = (h * 13) / 16;
  env->button_width = (w / NB_BUTTONS_LG) * 2 / 3;
  env->button_height = env->button_width * 2 / 4;

  // LEFT SIDE Buttons
  env->button_L_width = env->button_width / 2;
  env->button_L_height = env->button_width * 2 / 4;
  env->button_L_x = grid_x / 4 - env->button_L_width / 2;
  env->button_L_y = h / 4;
}

void init_texture_number(Env *env, SDL_Renderer *ren, SDL_Texture **t, int i,
                         SDL_Color c) {
  char digit[] = {48 + i, 0};  // Convert the int to a char using ASCII Table
  SDL_Surface *surf = TTF_RenderText_Blended(env->font, digit, c);
  *t = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
}

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  if (env == NULL) {
    exit(EXIT_FAILURE);
  }
  int w, h;
  env->satisfied =
      (SDL_Color){85, 107, 47, 0};  // Green color for a satisified cell
  env->unsatisfied = (SDL_Color){133, 6, 6, 0};
  env->error = (SDL_Color){255, 0, 0, 0};  // Red when a cell's status is error

  // INIT FONT
  env->font = TTF_OpenFont(FONT, FONTSIZE);
  if (!env->font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(env->font, TTF_STYLE_NORMAL);

  // INIT BACKGROUND
  SDL_GetWindowSize(win, &w, &h);
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  // INIT GAME
  if (argc == 1) {
    env->g = game_default();  // No file given so init game_default
  }
  if (argc == 2) {
    env->g = game_load(argv[1]);
  }

  compute_dims(env, w, h);  // Init of the sizes value in the env struct with
                            // the first window dimensions

  // Array of images containing the three different type of cells we have in the
  // game
  const char *imagePathColors[NB_CELL_COLORS] = {EMPTY_CELL, WHITE_CELL,
                                                 BLACK_CELL};
  // INIT CELL_IMAGES
  for (int i = 0; i < NB_CELL_COLORS; i++) {
    env->cell_colors[i] = IMG_LoadTexture(ren, imagePathColors[i]);
  }

  // GETTING THE GAME_CONSTRAINTS AS CHARS:
  for (int i = 0; i < 10; i++) {
    init_texture_number(env, ren, &env->digits_textures[0][i], i, env->error);
    init_texture_number(env, ren, &env->digits_textures[1][i], i,
                        env->unsatisfied);
    init_texture_number(env, ren, &env->digits_textures[2][i], i,
                        env->satisfied);
  }

  TTF_CloseFont(env->font);
  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  // Get current window size
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  // Compute the dimensions in case the window is resized.
  compute_dims(env, w, h);

  // Render background texture
  SDL_RenderCopy(ren, env->background, NULL, NULL); /* stretch it */

  // Computing the position of the grid
  int grid_x = (w / 2) - ((env->grid_width) / 2);
  int grid_y = (h / 2) - ((env->grid_height) / 2);

  // Drawing the cells
  for (int i = 0; i < env->game_nb_rows; i++) {
    for (int j = 0; j < env->game_nb_cols; j++) {
      color c = game_get_color(env->g, i, j);
      SDL_Rect rect;
      rect.x = grid_x + j * env->cell_width;
      rect.y = grid_y + i * env->cell_height;
      rect.w = env->cell_width * (0.92);
      rect.h = env->cell_height * (0.92);
      SDL_RenderCopy(ren, env->cell_colors[c], NULL, &rect);

      // Drawing the constraints
      double contraint_scale = 0.5;  // Size of a constraint compared to a cell.
      SDL_Rect destRect;
      destRect.w = env->cell_width * contraint_scale;
      destRect.h = env->cell_height * contraint_scale;
      destRect.x = rect.x + (rect.w / 2) - (destRect.w / 2);
      destRect.y = rect.y + (env->cell_height - destRect.h) / 2;
      constraint con = game_get_constraint(env->g, i, j);
      if (con != -1)
        SDL_RenderCopy(ren,
                       env->digits_textures[game_get_status(env->g, i, j)][con],
                       NULL, &destRect);
    }
  }

  // Drawing the Bottom placed buttons
  int ecart = env->button_width + (env->button_width / 3);
  const char *imagePath[NB_BUTTONS_LG] = {RESTART, SOLVE, LOAD, SAVE};
  for (int i = 0; i < NB_BUTTONS_LG; i++) {
    env->buttons_LG[i] =
        create_button(ren, imagePath[i], env->button_x, env->button_y);
    env->buttons_LG[i]->rect.h = env->button_height;
    env->buttons_LG[i]->rect.w = env->button_width;
    env->button_x += ecart;
    SDL_RenderCopy(ren, env->buttons_LG[i]->texture, NULL,
                   &(env->buttons_LG[i]->rect));
  }

  // drawing the left side placed buttons
  int ecartL = env->button_L_height + (env->button_L_height / 3);
  const char *imagePath2[NB_BUTTONS_L] = {UNDO, REDO};
  for (int i = 0; i < NB_BUTTONS_L; i++) {
    env->buttons_L[i] =
        create_button(ren, imagePath2[i], env->button_L_x, env->button_L_y);
    env->buttons_L[i]->rect.h = env->button_L_height;
    env->buttons_L[i]->rect.w = env->button_L_width;
    env->button_L_y += ecartL;
    SDL_RenderCopy(ren, env->buttons_L[i]->texture, NULL,
                   &(env->buttons_L[i]->rect));
  }
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  // Get current window size
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  // Compute the dimensions in case the window is resized.
  compute_dims(env, w, h);

  if (e->type == SDL_QUIT) {
    return true;
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    // Computing the position of the grid
    int grid_x = (w / 2) - ((env->grid_width) / 2);
    int grid_y = (h / 2) - ((env->grid_height) / 2);

    // Computing the coordinate of the mouseclick
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    // if the user clicked on a cell then play the move & modify its color
    for (int i = 0; i < (env->game_nb_rows); i++) {
      for (int j = 0; j < env->game_nb_cols; j++) {
        int x1 = grid_x + j * env->cell_width;
        int y1 = grid_y + i * env->cell_height;
        int x2 = x1 + env->cell_width;
        int y2 = y1 + env->cell_height;
        if (x1 <= mouse.x && mouse.x < x2 && y1 <= mouse.y && mouse.y < y2) {
          color c = game_get_color(env->g, i, j);
          c = (c + 2) % 3;
          game_play_move(env->g, i, j, c);
        }
      }
    }
    // If the user clicked on one of the bottom placed button then do its
    // function
    int ecart = env->button_width + (env->button_width / 3);
    for (int i = 0; i < NB_BUTTONS_LG; i++) {
      int x1 = (env->button_x) + (i * ecart);
      int y1 = env->button_y;
      int x2 = x1 + env->button_width;
      int y2 = y1 + env->button_height;
      if (x1 <= mouse.x && mouse.x < x2 && y1 <= mouse.y && mouse.y < y2) {
        switch (i) {
          case 0:
            game_restart(env->g);
            break;
          case 1:
            game_solve(env->g);
            break;
          case 2:
            game_load(LOAD_SAVE);
            break;
          case 3:
            game_save(env->g, LOAD_SAVE);
            break;
        }
      }
    }
    // If the user clicked on one of the left placed button then do its function
    int ecartL = env->button_L_height + (env->button_L_height / 3);
    for (int i = 0; i < NB_BUTTONS_L; i++) {
      int x1 = (env->button_L_x);
      int y1 = env->button_L_y + (i * ecartL);
      int x2 = x1 + env->button_L_width;
      int y2 = y1 + env->button_L_height;
      if (x1 <= mouse.x && mouse.x < x2 && y1 <= mouse.y && mouse.y < y2) {
        switch (i) {
          case 0:
            game_undo(env->g);
            break;
          case 1:
            game_redo(env->g);
            break;
        }
      }
    }
  }

  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_DestroyTexture(env->background);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 10; j++) SDL_DestroyTexture(env->digits_textures[i][j]);
    SDL_DestroyTexture(env->cell_colors[i]);
  }
  game_delete(env->g);
  free(env);

  /* **************************************************************** */
}
