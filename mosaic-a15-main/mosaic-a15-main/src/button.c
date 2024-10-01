#ifndef _BUTTON_H
#define _BUTTON_H
#include "Button.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "model.h"

Button *create_button(SDL_Renderer *ren, const char *image, int x, int y) {
  Button *button = (Button *)malloc(sizeof(Button));
  if (button == NULL) {
    exit(EXIT_FAILURE);
  }
  button->texture = IMG_LoadTexture(ren, image);
  if (button->texture == NULL) {
    free(button);
    exit(EXIT_FAILURE);
  }
  SDL_QueryTexture(button->texture, NULL, NULL, &(button->rect.w),
                   &(button->rect.h));
  button->rect.x = x;
  button->rect.y = y;

  return button;
}

#endif