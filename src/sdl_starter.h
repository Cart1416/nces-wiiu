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

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

struct Sprite {
    SDL_Texture *texture;
    SDL_Rect bounds;
    float hv = 0.0f;
    float vv = 0.0f;
    float fx = 0.0f;
    float fy = 0.0f;
    float angle;
    bool protectingToken = false;
    bool invulnerable = false;
    bool immobile = false;
    bool evil = false;
    int evilTimer = 0;
    SDL_GameController* controller = nullptr;
    int controllerId = -1;
    bool previousInvulnerable = false;
};

int startSDLSystems(SDL_Window *window, SDL_Renderer *renderer);

Sprite loadSprite(SDL_Renderer* renderer, const char* filePath, int positionX, int positionY, float vx = 0.0f, float vy = 0.0f);

Mix_Chunk *loadSound(const char *filePath);

Mix_Music *loadMusic(const char *filePath);

void updateTextureText(SDL_Texture *&texture, const char *text, TTF_Font *&fontSquare, SDL_Renderer *renderer, SDL_Color fontColor = {255, 255, 255, 255});

void stopSDLSystems();
