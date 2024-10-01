
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "model.h"

typedef struct {
  SDL_Texture* texture;
  SDL_Rect rect;
} Button;

Button* create_button(SDL_Renderer* ren, const char* image, int x, int y);

#endif
