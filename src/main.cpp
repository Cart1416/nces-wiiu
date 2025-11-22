#include "sdl_starter.h"
#include <time.h>
#include <unistd.h> // chdir header
#include <romfs-wiiu.h>
#include <whb/proc.h>
#include <string>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *controller = nullptr;

const int PLAYER_SPEED = 600;

Sprite playerSprite;

Mix_Music *music = nullptr;
Mix_Chunk *sound = nullptr;

bool isGamePaused = false;

SDL_Texture *pauseTexture = nullptr;
SDL_Rect pauseBounds;

SDL_Texture *scoreTexture = nullptr;
SDL_Rect scoreBounds;

int score = 0;

TTF_Font *font = nullptr;

bool isGameRunning = true;

SDL_Rect ball = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 32, 32};

int ballVelocityX = 400;
int ballVelocityY = 400;

int colorIndex = 0;

SDL_Color colors[] = {
    {128, 128, 128, 0}, // gray
    {255, 255, 255, 0}, // white
    {255, 0, 0, 0},     // red
    {0, 255, 0, 0},     // green
    {0, 0, 255, 0},     // blue
    {255, 255, 0, 0},   // brown
    {0, 255, 255, 0},   // cyan
    {255, 0, 255, 0},   // purple
};

void handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isGameRunning = false;
            break;
        }

        if (event.type == SDL_JOYBUTTONDOWN)
        {
            if (event.jbutton.button == BUTTON_MINUS)
            {
                isGameRunning = false;
                break;
            }

            if (event.jbutton.button == BUTTON_PLUS)
            {
                isGamePaused = !isGamePaused;
                Mix_PlayChannel(-1, sound, 0);
            }
        }
    }
}

int getRandomNumberBetweenRange(int min, int max)
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void update(float deltaTime)
{
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && playerSprite.bounds.y > 0)
    {
        playerSprite.bounds.y -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && playerSprite.bounds.y < SCREEN_HEIGHT - playerSprite.bounds.h)
    {
        playerSprite.bounds.y += PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && playerSprite.bounds.x > 0)
    {
        playerSprite.bounds.x -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && playerSprite.bounds.x < SCREEN_WIDTH - playerSprite.bounds.w)
    {
        playerSprite.bounds.x += PLAYER_SPEED * deltaTime;
    }

    if (ball.x < 0 || ball.x > SCREEN_WIDTH - ball.w)
    {
        ballVelocityX *= -1;

        colorIndex = getRandomNumberBetweenRange(0, 5);
    }

    else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - ball.h)
    {
        ballVelocityY *= -1;

        colorIndex = getRandomNumberBetweenRange(0, 5);
    }

    else if (SDL_HasIntersection(&playerSprite.bounds, &ball))
    {
        ballVelocityX *= -1;
        ballVelocityY *= -1;

        colorIndex = getRandomNumberBetweenRange(0, 5);

        Mix_PlayChannel(-1, sound, 0);

        score++;

        std::string scoreString = "SCORE: " + std::to_string(score);

        updateTextureText(scoreTexture, scoreString.c_str(), font, renderer);
    }

    ball.x += ballVelocityX * deltaTime;
    ball.y += ballVelocityY * deltaTime;
}

void renderSprite(Sprite &sprite)
{
    SDL_RenderCopy(renderer, sprite.texture, NULL, &sprite.bounds);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, 255);

    SDL_RenderFillRect(renderer, &ball);

    renderSprite(playerSprite);

    if (isGamePaused)
    {
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseBounds);
    }

    SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreBounds.w, &scoreBounds.h);
    scoreBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    scoreBounds.y = scoreBounds.h / 2;
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreBounds);

    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
    WHBProcInit();
    romfsInit();
    chdir("romfs:/");

    window = SDL_CreateWindow("Wii U SDL Starter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (startSDLSystems(window, renderer) > 0)
    {
        return 1;
    }

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);

    //the controller is always connected in this console.
    controller = SDL_GameControllerOpen(0);

    playerSprite = loadSprite(renderer, "sprites/alien_1.png", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2);

    font = TTF_OpenFont("fonts/LeroyLetteringLightBeta01.ttf", 36);

    // render text as texture
    updateTextureText(scoreTexture, "SCORE: 0", font, renderer);

    updateTextureText(pauseTexture, "GAME PAUSED", font, renderer);

    SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseBounds.w, &pauseBounds.h);
    pauseBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    pauseBounds.y = 200;

    // load music and sounds from files
    sound = loadSound("sounds/pop1.wav");
    music = loadMusic("music/background.ogg");

    Mix_PlayMusic(music, -1);

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    while (isGameRunning && WHBProcIsRunning())
    {
        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        handleEvents();

        if (!isGamePaused)
        {
            update(deltaTime);
        }

        render();
    }

    Mix_FreeMusic(music);
    Mix_FreeChunk(sound);
    SDL_DestroyTexture(playerSprite.texture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    stopSDLSystems();
    romfsExit();
    WHBProcShutdown();

    return 0;
}
