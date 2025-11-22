#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// some wii u buttons
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define BUTTON_STICKL 4
#define BUTTON_STICKR 5
#define BUTTON_L 6
#define BUTTON_R 7
#define BUTTON_ZL 8
#define BUTTON_ZR 9
#define BUTTON_PLUS 10
#define BUTTON_MINUS 11
#define BUTTON_LEFT 12
#define BUTTON_UP 13
#define BUTTON_RIGHT 14
#define BUTTON_DOWN 15
#define BUTTON_COUNT 16

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect bounds;
} Sprite;

int startSDLSystems(SDL_Window *window, SDL_Renderer *renderer);

Sprite loadSprite(SDL_Renderer *renderer, const char *filePath, int positionX, int positionY);

Mix_Chunk *loadSound(const char *filePath);

Mix_Music *loadMusic(const char *filePath);

void updateTextureText(SDL_Texture *&texture, const char *text, TTF_Font *&fontSquare, SDL_Renderer *renderer);

void stopSDLSystems();