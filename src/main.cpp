#include "sdl_starter.h"      // Custom header file for SDL helper functions
#include <time.h>             // For random number seeding and time functions
#include <unistd.h>           // For chdir() to change directory
#include <romfs-wiiu.h>       // Wii U ROM filesystem functions
#include <whb/proc.h>         // Wii U process handling
#include <string>             // C++ string support
#include <stdlib.h> // rand, srand
#include <vector>
#include <cmath>
#include <algorithm>

const std::string gameModeNames[] = {
    "Nic Cage Eats Stuff",
    "EASY Nic Cage Eats Stuff",
    "IMPOSSIBLE Nic Cage Eats Stuff",
    "Nic Cage Eats Stuff 2"
};

const std::string tokenToCollectText[] = {
    "Chicken eaten: ",
    "Chicken eaten: ",
    "Chicken eaten: ",
    "Chicken eaten: "
};

const std::string enemyToCollectText[] = {
    "Celery eaten: ",
    "Celery eaten: ",
    "Celery eaten: ",
    "Celery eaten: "
};

const int maxEnemyEaten[] = {
    3,
    999,
    10,
    3
};

const std::string gameOverText[] = {
    "You died! Press A to restart or - to change game.",
    "How did you die? Press A to restart or - to change game.",
    "You are trash lol. Press A to restart or - to change game.",
    "GAME OVER. Press A to restart or - to change game."
};

const char* playerImage[] = {
    "sprites/NicCageFace.png",
    "sprites/NicCageFace.png",
    "sprites/NicCageFace.png",
    "sprites/NicCageFace.png"
};

const char* playerTransparentImage[] = {
    "sprites/NicCageFaceTransparent.png",
    "sprites/NicCageFaceTransparent.png",
    "sprites/NicCageFaceTransparent.png",
    "sprites/NicCageFaceTransparent.png"
};

const char* tokenImage[] = {
    "sprites/chicken.png",
    "sprites/chicken.png",
    "sprites/chicken.png",
    "sprites/chicken.png"
};

const char* enemyImage[] = {
    "sprites/celery.png",
    "sprites/celery.png",
    "sprites/celery.png",
    "sprites/celery.png"
};

const int tokenCount[] = {
    1,
    5,
    1,
    1
};

const std::vector<std::vector<std::string>> gameModeModifiers = {
    {},
    {"noEnemy"},
    {"spawnEnemyOnMove"},
    {"angryCelery", "blackEndScreen", "altUI", "enemiesBounce", "randomSizeEnemies", "noCircle"}
};

const int playerSpeed[] = {
    250,
    500,
    250,
    250
};




int rng(int min, int max) {
    return min + rand() % (max - min + 1);
}

// SDL objects
SDL_Window *window = nullptr;           // The game window
SDL_Renderer *renderer = nullptr;       // The rendering context for the window
SDL_GameController *controller = nullptr; // The game controller (Wii U Gamepad)
SDL_GameController *controller1 = nullptr; // The game controller (Wii U Pro Controller 1)

// Game constants
int PLAYER_SPEED = 250;           // Player movement speed in pixels/sec

// Player sprite
Sprite playerSprite;                    // Custom struct representing the player
std::vector<Sprite> players;            // players
SDL_Rect mouth;                    // Custom struct representing the player's mouth
std::vector<SDL_Rect> mouths;        // mouths

// Audio
Mix_Music *music = nullptr;             // Background music
Mix_Chunk *sound = nullptr;             // Short sound effects

// Game state
bool isGamePaused = false;              // Flag for pause state
bool isGameRunning = true;              // Main loop control flag
std::string currentScreen = "menu";
size_t currentGameMode = 0; // 0 is classic, 1 is easy, 2 is impossible


// Pause screen
SDL_Texture *pauseTexture = nullptr;    // Texture for pause message
SDL_Rect pauseBounds;                   // Position and size of pause message

// Score display
SDL_Texture *tokenseatenTexture = nullptr;    // Texture for the tokenseaten
SDL_Rect tokenseatenBounds;                   // Position and size of tokenseaten
int tokenseaten = 0;                          // Player tokenseaten


// Enemy eaten display
SDL_Texture *enemyEatenTexture = nullptr;    // Texture for the tokenseaten
SDL_Rect enemyEatenBounds;                   // Position and size of tokenseaten
int enemyEaten = 0;                          // Enemy tokenseaten

// misc display1
SDL_Texture *miscTexture1 = nullptr;    // Texture for the tokenseaten
SDL_Rect miscBounds1;                   // Position and size of tokenseaten

// Font for text rendering
TTF_Font *font = nullptr;               // Font used for tokenseaten/pause text

// Enemy (ball) properties
SDL_Rect ball = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 32, 32}; // Ball position & size
int ballVelocityX = 400;                // Ball velocity X (pixels/sec)
int ballVelocityY = 400;                // Ball velocity Y (pixels/sec)

// Enemy sprite
Sprite enemySprite;                    // Custom struct representing the enemy
int evilEnemyTimer = 1800;

std::vector<Sprite> enemies;
float enemySpeedMin = 120.0f;
float enemySpeedMax = 240.0f;

std::vector<Sprite> tokens;

// Ball color handling
int colorIndex = 0;                     // Index of current ball color
SDL_Color colors[] = {
    {128, 128, 128, 0}, // gray
    {255, 255, 255, 0}, // white
    {255, 0, 0, 0},     // red
    {0, 128, 0, 0},     // green
    {0, 0, 255, 0},     // blue
    {255, 255, 0, 0},   // brown
    {0, 255, 255, 0},   // cyan
    {255, 0, 255, 0},   // purple
    {0, 0, 0, 0}, // black
};

// ------------------ EVENT HANDLING ------------------
void handleEvents() {
    SDL_Event event;

    // Poll all events in the queue
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { // Window close event
            isGameRunning = false;    // Exit main loop
            break;
        }

        if (event.type == SDL_JOYBUTTONDOWN) { // Controller button pressed
            if (event.jbutton.button == BUTTON_MINUS) { // Minus button quits back to game select
                currentScreen = "menu";
                isGamePaused = false;
            }

            if (event.jbutton.button == BUTTON_PLUS) { // Plus button toggles pause
                isGamePaused = !isGamePaused;
                Mix_PlayChannel(-1, sound, 0); // Play sound effect
            }
        }
        if (event.type == SDL_CONTROLLERDEVICEADDED) {
            // Controller was connected!
            // refresh all controllers
            //SDL_GameControllerClose(controller);
            //controller = SDL_GameControllerOpen(0);
            //SDL_GameControllerClose(controller1);
            //controller1 = SDL_GameControllerOpen(1);
            auto* firstConnectedController = SDL_GameControllerOpen(0);
            auto* secondConnectedController = SDL_GameControllerOpen(1);
            if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 0) {
                controller = firstConnectedController;
            } else if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 1) {
                controller1 = firstConnectedController;
            }
            if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 0) {
                controller = secondConnectedController;
            } else if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 1) {
                controller1 = secondConnectedController;
            }
            for (auto& player : players) {
                if (player.controllerId == 0) {
                    player.controller = controller;
                } else if (player.controllerId == 1) {
                    player.controller = controller1;
                }
            }
        }

        if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
            // Controller was removed!
            // refresh all controllers
            //SDL_GameControllerClose(controller);
            //controller = SDL_GameControllerOpen(0);
            //SDL_GameControllerClose(controller1);
            //controller1 = SDL_GameControllerOpen(1);
            auto* firstConnectedController = SDL_GameControllerOpen(0);
            auto* secondConnectedController = SDL_GameControllerOpen(1);
            if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 0) {
                controller = firstConnectedController;
            } else if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 1) {
                controller1 = firstConnectedController;
            }
            if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 0) {
                controller = secondConnectedController;
            } else if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 1) {
                controller1 = secondConnectedController;
            }
            for (auto& player : players) {
                if (player.controllerId == 0) {
                    player.controller = controller;
                } else if (player.controllerId == 1) {
                    player.controller = controller1;
                }
            }
        }
    }
}

// ------------------ UTILITY ------------------
int getRandomNumberBetweenRange(int min, int max) {
    // Return a random integer between min and max inclusive
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

float rngFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

bool contains(const std::vector<std::string>& vec, const std::string& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

// Function to add an enemy
void addEnemyCustom(SDL_Renderer* renderer, const char* filePath, int x, int y, float hv = 0.0f, float vv = 0.0f) {
    // Load the sprite with optional speed
    Sprite newEnemy = loadSprite(renderer, filePath, x, y, hv, vv);

    if (contains(gameModeModifiers[currentGameMode], "randomSizeEnemies")) {
        float enemySizeMultiplier = rngFloat(0.5f, 2.0f);
        newEnemy.bounds.w *= enemySizeMultiplier;
        newEnemy.bounds.h *= enemySizeMultiplier;
    }

    // Add it to the dynamic vector
    enemies.push_back(newEnemy);
}

// Function to add a token
void addTokenCustom(SDL_Renderer* renderer, const char* filePath, int x, int y, float hv = 0.0f, float vv = 0.0f) {
    // Load the sprite with optional speed
    Sprite newToken = loadSprite(renderer, filePath, x, y, hv, vv);

    // Add it to the dynamic vector
    tokens.push_back(newToken);
}

void addPlayerCustom(SDL_Renderer* renderer, const char* filePath, int x, int y, SDL_GameController* controller = controller, int controllerId = 0) {
    Sprite newPlayer = loadSprite(renderer, filePath, x, y);
    newPlayer.controller = controller;
    newPlayer.controllerId = controllerId;

    players.push_back(newPlayer);

    // Setup mouth rectangle relative to player's position
    SDL_Rect mouth;
    mouth.x = x + 27;   // Adjust offset as done in your main loop
    mouth.y = y + 88;
    mouth.w = 40;
    mouth.h = 20;

    mouths.push_back(mouth);
}

// Function to add an enemy
void addEnemy() {
    int enemyLen = static_cast<int>(enemies.size());
    if (!(contains(gameModeModifiers[currentGameMode], "noEnemy")) && enemyLen < 200) {
        addEnemyCustom(renderer, enemyImage[currentGameMode], rng(0, SCREEN_WIDTH), rng(0, SCREEN_HEIGHT), rngFloat(enemySpeedMin, enemySpeedMax), rngFloat(enemySpeedMin, enemySpeedMax));
    }
}

// Function to add a token
void addToken() {
    addTokenCustom(renderer, tokenImage[currentGameMode], rng(0, SCREEN_WIDTH), rng(0, SCREEN_HEIGHT), 0.0f, 0.0f);
}

void addPlayer(SDL_GameController* controller = controller, int controllerId = 0) {
    addPlayerCustom(renderer, playerImage[currentGameMode], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, controller, controllerId);
}

// Helper funcs
void circleAroundObject(Sprite& center, Sprite& orbiter, float radius)
{
    // If angle was never initialized (0 means allowed, so detect NaN instead)
    if (std::isnan(orbiter.angle))
        orbiter.angle = 0.0f;

    // Compute new float position
    orbiter.fx = center.fx + std::cos(orbiter.angle) * radius;
    orbiter.fy = center.fy + std::sin(orbiter.angle) * radius;

    // Update angle by +0.2
    //orbiter.angle += 0.2f;

    // Wrap at 2π
    //const float TAU = 2.0f * M_PI;
    //if (orbiter.angle >= TAU)
        //orbiter.angle -= TAU;

    orbiter.angle = fmodf(orbiter.angle + 0.04, 2*M_PI);

    // Update render position (convert float → int)
    orbiter.bounds.x = static_cast<int>(orbiter.fx);
    orbiter.bounds.y = static_cast<int>(orbiter.fy);
}

std::string horizontalDirection(const Sprite& object1, const Sprite& object2)
{
    float horizontal = object1.fx - object2.fx;

    if (horizontal > 0)
        return "right";
    else if (horizontal < 0)
        return "left";
    else
        return "equal";
}

std::string verticalDirection(const Sprite& object1, const Sprite& object2)
{
    float vertical = object1.fy - object2.fy;

    if (vertical > 0)
        return "top";
    //else if (vertical < 0)
        //return "bottom";
    else
        //return "equal";  // optional, for the rare case of exact same y
        return "bottom";
}


void attract(Sprite& object1, Sprite& object2)
{
    std::string h = horizontalDirection(object1, object2);
    std::string v = verticalDirection(object1, object2);

    // Reverse horizontal velocity if moving in the wrong direction
    if ((h == "left"  && object2.hv > 0) ||
        (h == "right" && object2.hv < 0))
    {
        object2.hv = -object2.hv;
    }

    // Reverse vertical velocity if moving in the wrong direction
    if ((v == "top"    && object2.vv < 0) ||
        (v == "bottom" && object2.vv > 0))
    {
        object2.vv = -object2.vv;
    }
}

float distance(const Sprite& object1, const Sprite& object2) {
    float dx = object1.fx - object2.fx;
    float dy = object1.fy - object2.fy;
    return std::sqrt(dx * dx + dy * dy);
}

void restartGame() {
    enemies.clear();
    tokens.clear();
    players.clear();
    mouths.clear();
    evilEnemyTimer = 1800;
    addEnemy();
    for (int i = 0; i < tokenCount[currentGameMode]; i++) {
        addToken();
    }
    playerSprite = loadSprite(renderer, playerImage[currentGameMode], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    PLAYER_SPEED = playerSpeed[currentGameMode];
    addPlayer(controller, 0);
    addPlayer(controller1, 1);
    for (auto& player : players) {
        if (player.controllerId == 0) {
            player.controller = controller;
        } else if (player.controllerId == 1) {
            player.controller = controller1;
        }
    }
    enemyEaten = 0;
    tokenseaten = 0;
}

bool previousInvulnerable = false;
bool previousLeft = false;
bool previousRight = false;

// ------------------ GAME LOGIC ------------------
void update(float deltaTime) {
    // Move player based on controller input
    if (currentScreen == "menu") {
        if (SDL_GameControllerGetPlayerIndex(controller) >= 0 && controller != nullptr && SDL_GameControllerGetAttached(controller)) {
            if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A)) {
                currentScreen = "game";            
                isGamePaused = false;
                restartGame();
            }
            if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
                if (currentGameMode > 0 && !previousLeft) {
                    currentGameMode--;
                }
                previousLeft = true;
            } else {
                previousLeft = false;
            }
            if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
                //int gameModeLength = sizeof(gameModeNames);
                size_t gameModeLength = sizeof(gameModeNames) / sizeof(gameModeNames[0]);
                if (currentGameMode < (gameModeLength - 1) && !previousRight) {
                    currentGameMode++;
                }
                previousRight = true;
            } else {
                previousRight = false;
            }
        } else {
            if (SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_A)) {
                currentScreen = "game";            
                isGamePaused = false;
                restartGame();
            }
            if (SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
                if (currentGameMode > 0 && !previousLeft) {
                    currentGameMode--;
                }
                previousLeft = true;
            } else {
                previousLeft = false;
            }
            if (SDL_GameControllerGetButton(controller1, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
                //int gameModeLength = sizeof(gameModeNames);
                size_t gameModeLength = sizeof(gameModeNames) / sizeof(gameModeNames[0]);
                if (currentGameMode < (gameModeLength - 1) && !previousRight) {
                    currentGameMode++;
                }
                previousRight = true;
            } else {
                previousRight = false;
            }
        }
    }
    if (currentScreen == "game") {
        int playerI2 = 0;
        for (auto& playerSprite : players) {
            if (SDL_GameControllerGetPlayerIndex(playerSprite.controller) >= 0 && playerSprite.controller != nullptr && SDL_GameControllerGetAttached(playerSprite.controller)) {
                playerSprite.controllerId = SDL_GameControllerGetPlayerIndex(playerSprite.controller);
            }
            auto currentController = playerSprite.controller;
            if (!playerSprite.immobile && enemyEaten < maxEnemyEaten[currentGameMode]) {
                if (SDL_GameControllerGetButton(currentController, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
                    playerSprite.bounds.y -= PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.y < -80) { // Wrap around top -> bottom
                        playerSprite.bounds.y = SCREEN_HEIGHT - playerSprite.bounds.h;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                } else if (static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTY) / 32768.0f) < -0.1f) {
                    playerSprite.bounds.y += static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTY)) / 32768 * PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.y < -80) { // Wrap around top -> bottom
                        playerSprite.bounds.y = SCREEN_HEIGHT - playerSprite.bounds.h;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                }
                if (SDL_GameControllerGetButton(currentController, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
                    playerSprite.bounds.y += PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.y > SCREEN_HEIGHT - playerSprite.bounds.h + 80) { // Wrap bottom -> top
                        playerSprite.bounds.y = 0;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                } else if (static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTY) / 32768.0f) > 0.1f) {
                    playerSprite.bounds.y += static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTY)) / 32768 * PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.y > SCREEN_HEIGHT - playerSprite.bounds.h + 80) { // Wrap bottom -> top
                        playerSprite.bounds.y = 0;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                }
                if (SDL_GameControllerGetButton(currentController, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
                    playerSprite.bounds.x -= PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.x < -80) {
                        playerSprite.bounds.x = SCREEN_WIDTH;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                } else if (static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTX) / 32768.0f) < -0.1f) {
                    playerSprite.bounds.x += static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTX)) / 32768 * PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.x < -80) {
                        playerSprite.bounds.x = SCREEN_WIDTH;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                }
                if (SDL_GameControllerGetButton(currentController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
                    playerSprite.bounds.x += PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.x > SCREEN_WIDTH - playerSprite.bounds.w + 80) {
                        playerSprite.bounds.x = 0;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                } else if (static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTX) / 32768.0f) > 0.1f) {
                    playerSprite.bounds.x += static_cast<float>(SDL_GameControllerGetAxis(currentController, SDL_CONTROLLER_AXIS_LEFTX)) / 32768 * PLAYER_SPEED * deltaTime;
                    if (playerSprite.bounds.x > SCREEN_WIDTH - playerSprite.bounds.w + 80) {
                        playerSprite.bounds.x = 0;
                    }
                    if (contains(gameModeModifiers[currentGameMode], "spawnEnemyOnMove")) {
                        addEnemy();
                    }
                }
            }
            mouths[playerI2].x = playerSprite.bounds.x + 27;
            mouths[playerI2].y = playerSprite.bounds.y + 88;
            mouths[playerI2].w = 40;
            mouths[playerI2].h = 20;
            if (SDL_GameControllerGetButton(currentController, SDL_CONTROLLER_BUTTON_A)) {
                if (playerSprite.previousInvulnerable == false) {
                    playerSprite.texture = IMG_LoadTexture(renderer, playerTransparentImage[currentGameMode]);
                }
                playerSprite.invulnerable = true;
                playerSprite.immobile = true;
                playerSprite.previousInvulnerable = true;
            } else {
                if (playerSprite.previousInvulnerable == true) {
                    playerSprite.texture = IMG_LoadTexture(renderer, playerImage[currentGameMode]);
                }
                playerSprite.invulnerable = false;
                playerSprite.immobile = false;
                playerSprite.previousInvulnerable = false;
            }
            playerI2++;
        }
        if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A) && enemyEaten >= maxEnemyEaten[currentGameMode]) {
            restartGame();
        }
        
        int playerI = 0;
        for (auto& playerSprite : players) {
            if (playerSprite.controller != nullptr && SDL_GameControllerGetAttached(playerSprite.controller)) {
                // enemy collision with player
                for (auto& enemy : enemies) {
                    if (SDL_HasIntersection(&mouths[playerI], &enemy.bounds) && !playerSprite.invulnerable) {
                        enemyEaten++;                        // Increment enemy eaten
                        enemy.fx = rng(0, SCREEN_WIDTH);
                        enemy.fy = rng(0, SCREEN_HEIGHT);
                    }
                }

                // token collision with player
                for (auto& token : tokens) {
                    if (SDL_HasIntersection(&mouths[playerI], &token.bounds)) {
                        Mix_PlayChannel(-1, sound, 0); // Play collision sound
                        tokenseaten++;                        // Increment tokenseaten
                        token.fx = rng(0, SCREEN_WIDTH - 10);
                        token.fy = rng(0, SCREEN_HEIGHT - 10);
                        if (tokenseaten % 3 == 0) {
                            addEnemy();
                        }
                    }
                }
            }
            playerI++;
        }

        if (contains(gameModeModifiers[currentGameMode], "angryCelery")) {
            evilEnemyTimer--;
            if (evilEnemyTimer <= 0) {
                enemies[0].evil = true;
                enemies[0].evilTimer = 900;
                evilEnemyTimer = 1800;
            }
        }

        // update the enemies
        int i = 0;
        for (auto& enemy : enemies) {
            int enemyLen = static_cast<int>(enemies.size());
            int tokenLen = static_cast<int>(tokens.size());
            if (enemyLen % 4 == 0 && !(contains(gameModeModifiers[currentGameMode], "noCircle"))) {
                enemy.protectingToken = true;
            } else {
                enemy.protectingToken = false;
            }
            if (!enemy.protectingToken) {
                enemy.fx += enemy.hv * deltaTime;
                enemy.fy += enemy.vv * deltaTime;
            } else {
                int tokenIToCircle = static_cast<int>(std::floor(static_cast<float>(i) / (static_cast<float>(enemyLen) / static_cast<float>(tokenLen))));
                int distanceToToken = distance(enemy, tokens[tokenIToCircle]);
                if (distanceToToken >= 200) {
                    // Attract the enemy towards the token
                    attract(enemy, tokens[tokenIToCircle]);
                    // Update float positions
                    enemy.fx += enemy.hv * deltaTime;
                    enemy.fy += enemy.vv * deltaTime;
                } else {
                    // Circle around the token
                    circleAroundObject(tokens[tokenIToCircle], enemy, 190);
                }
            }

            if (contains(gameModeModifiers[currentGameMode], "enemiesBounce")) {
                int ii = 0;
                for (auto& enemy2 : enemies) {
                    if (SDL_HasIntersection(&enemy.bounds, &enemy2.bounds) && i != ii) {
                        enemy.hv = -enemy.hv;
                        enemy.vv = -enemy.vv;
                        enemy.fx += enemy.hv * deltaTime * 3;
                        enemy.fy += enemy.vv * deltaTime * 3;
                    }
                    ii++;
                }
            }

            if (enemy.evilTimer > 0) {
                --enemy.evilTimer;
                if (enemy.evilTimer == 899) {
                    enemy.texture = IMG_LoadTexture(renderer, "sprites/red_celery.png");
                    enemy.hv *= 3;
                    enemy.vv *= 3;
                } else if (enemy.evilTimer == 1) {
                    enemy.texture = IMG_LoadTexture(renderer, enemyImage[currentGameMode]);
                    enemy.hv /= 3;
                    enemy.vv /= 3;
                }
            }

            // Bounce off left/right edges
            if (enemy.fx < 0) {
                enemy.fx = 0;       // prevent going offscreen
                enemy.hv *= -1;     // reverse X velocity
            }
            else if (enemy.fx > SCREEN_WIDTH - enemy.bounds.w) {
                enemy.fx = SCREEN_WIDTH - enemy.bounds.w;
                enemy.hv *= -1;
            }

            // Bounce off top/bottom edges
            if (enemy.fy < 0) {
                enemy.fy = 0;
                enemy.vv *= -1;     // reverse Y velocity
            }
            else if (enemy.fy > SCREEN_HEIGHT - enemy.bounds.h) {
                enemy.fy = SCREEN_HEIGHT - enemy.bounds.h;
                enemy.vv *= -1;
            }

            // Update SDL_Rect for rendering
            enemy.bounds.x = static_cast<int>(enemy.fx);
            enemy.bounds.y = static_cast<int>(enemy.fy);
            enemy.bounds.x = enemy.fx;
            enemy.bounds.y = enemy.fy;
            i++;
        }

        // Move the tokens
        for (auto& token : tokens) {
            //token.fx += token.hv * deltaTime;
            //token.fy += token.vv * deltaTime;
            token.bounds.x = token.fx;
            token.bounds.y = token.fy;
        }
    }
}

// ------------------ RENDERING ------------------
void renderSprite(Sprite &sprite) {
    // Draw the sprite texture at its current bounds
    SDL_RenderCopy(renderer, sprite.texture, NULL, &sprite.bounds);
}

void drawText(SDL_Renderer* renderer, std::string text, int x, int y, SDL_Color color = colors[8], std::string positioning = "") {
    SDL_Texture *textTexture = nullptr;
    updateTextureText(textTexture, text.c_str(), font, renderer, color);
    SDL_Rect textBounds;

    SDL_QueryTexture(textTexture, NULL, NULL, &textBounds.w, &textBounds.h);
    textBounds.y = y;
    textBounds.x = x;
    if (positioning == "center") {
        textBounds.x = x - textBounds.w / 2;
    } else if (positioning == "right") {
        textBounds.x = x - textBounds.w;
    }
    SDL_RenderCopy(renderer, textTexture, NULL, &textBounds);
    SDL_DestroyTexture(textTexture);
}

void render() {
    int backgroundColors = 255;
    if (currentScreen == "game" && contains(gameModeModifiers[currentGameMode], "blackEndScreen") && enemyEaten >= maxEnemyEaten[currentGameMode]) {
        backgroundColors = 0;
    }
    SDL_SetRenderDrawColor(renderer, backgroundColors, backgroundColors, backgroundColors, 255); // white background
    SDL_RenderClear(renderer);

    drawText(renderer, std::to_string(SDL_GameControllerGetAttached(controller)), 0, 200);
    drawText(renderer, std::to_string(SDL_GameControllerGetAttached(controller1)), 0, 300);

    drawText(renderer, std::to_string(SDL_GameControllerGetPlayerIndex(controller)), 100, 200);
    drawText(renderer, std::to_string(SDL_GameControllerGetPlayerIndex(controller1)), 100, 300);

    if (currentScreen == "menu") {
        // Update selected game text
        drawText(renderer, "Game: " + gameModeNames[currentGameMode], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, colors[8], "center");

        // Update navigation text
        drawText(renderer, "A: Select    D-PAD: Navigate", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, colors[8], "center");
    }
    if (currentScreen == "game") {
        // Draw player
        if (enemyEaten < maxEnemyEaten[currentGameMode]) {
            //renderSprite(playerSprite);
            int i = 0;
            for (auto& player : players) {
                drawText(renderer, std::to_string(SDL_GameControllerGetAttached(player.controller)), 50, 200 + (i * 100));
                drawText(renderer, std::to_string(SDL_GameControllerGetPlayerIndex(player.controller)), 150, 200 + (i * 100));
                drawText(renderer, std::to_string(player.controllerId), 200, 200 + (i * 100));
                if (player.controller != nullptr && SDL_GameControllerGetAttached(player.controller)) {
                    renderSprite(player); // same function as before
                    if (controller1 != nullptr && SDL_GameControllerGetAttached(controller1)) {
                        drawText(renderer, std::to_string(SDL_GameControllerGetPlayerIndex(player.controller)), player.bounds.x, player.bounds.y + player.bounds.w);
                    }
                }
                i++;
            }

            for (auto& enemy : enemies) {
                renderSprite(enemy); // same function as before
            }

            for (auto& token : tokens) {
                renderSprite(token); // same function as before
            }
        }

        // Draw pause message if game is paused
        if (isGamePaused) {
            SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseBounds);
        }

        // Update celery eaten text
        std::string enemyEatenString = "";
        int enemyEatenColor = 3;
        if (enemyEaten < maxEnemyEaten[currentGameMode]) {
            enemyEatenString = enemyToCollectText[currentGameMode] + std::to_string(enemyEaten) + "/" + std::to_string(maxEnemyEaten[currentGameMode]);
        } else {
            enemyEatenString = gameOverText[currentGameMode];
            if (contains(gameModeModifiers[currentGameMode], "blackEndScreen")) {
                enemyEatenColor = 1;
            }
        }
        int enemyEatenX = 32;
        std::string enemyEatenPosition = "";
        if (contains(gameModeModifiers[currentGameMode], "altUI")) {
            if (enemyEaten >= maxEnemyEaten[currentGameMode]) {
                enemyEatenX = 0;
            } else {
                enemyEatenX = SCREEN_WIDTH - 400;
                enemyEatenPosition = "right";
            }
        }
        drawText(renderer, enemyEatenString, enemyEatenX, 0, colors[enemyEatenColor], enemyEatenPosition);

        // Update tokenseaten text
        int tokensEatenColor = 8;
        if (enemyEaten >= maxEnemyEaten[currentGameMode] && contains(gameModeModifiers[currentGameMode], "blackEndScreen")) {
            tokensEatenColor = 1;
        }
        int tokensEatenX = 32;
        int tokensEatenY = 40;
        SDL_QueryTexture(tokenseatenTexture, NULL, NULL, &tokenseatenBounds.w, &tokenseatenBounds.h);
        if (contains(gameModeModifiers[currentGameMode], "altUI")) {
            tokensEatenX = SCREEN_WIDTH - tokenseatenBounds.w;
            tokensEatenY = 0;
        }
        drawText(renderer, tokenToCollectText[currentGameMode] + std::to_string(tokenseaten), tokensEatenX, tokensEatenY, colors[tokensEatenColor], enemyEatenPosition);

        // Update misc1 text
        std::string miscString1 = "";
        int enemyLen = static_cast<int>(enemies.size());
        if (enemyLen >= 200) {
            miscString1 = "Enemy limit of 200 reached!";
        }
        if (miscString1 != "") {
            drawText(renderer, miscString1, 32, 80, colors[8]);
        }
    }
    // Present everything on screen
    SDL_RenderPresent(renderer);
}

// ------------------ MAIN FUNCTION ------------------
int main(int argc, char **argv) {
    WHBProcInit();       // Initialize Wii U process system
    romfsInit();         // Initialize ROM filesystem
    chdir("romfs:/");    // Change working directory to ROM filesystem

    // Create SDL window and renderer
    window = SDL_CreateWindow("Nic Cage Eats Stuff", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (startSDLSystems(window, renderer) > 0) { // Custom SDL initialization
        return 1;
    }

    SDL_JoystickEventState(SDL_ENABLE); // Enable joystick events
    SDL_JoystickOpen(0);                // Open the first joystick

    // Controller always connected on this console
    auto* firstConnectedController = SDL_GameControllerOpen(0);
    auto* secondConnectedController = SDL_GameControllerOpen(1);
    if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 0) {
        controller = firstConnectedController;
    } else if (SDL_GameControllerGetPlayerIndex(firstConnectedController) == 1) {
        controller1 = firstConnectedController;
    }
    if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 0) {
        controller = secondConnectedController;
    } else if (SDL_GameControllerGetPlayerIndex(secondConnectedController) == 1) {
        controller1 = secondConnectedController;
    }

    srand(time(NULL));

    // Load player sprite
    playerSprite = loadSprite(renderer, "sprites/NicCageFace.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    enemySprite = loadSprite(renderer, "sprites/celery.png", rng(0, SCREEN_WIDTH), rng(0, SCREEN_HEIGHT));
    //addEnemy();
    //addToken();
    restartGame();

    // Load font
    font = TTF_OpenFont("fonts/cour.ttf", 36);

    // Initialize tokenseaten and pause textures
    updateTextureText(enemyEatenTexture, "Celery Eaten: 0/3", font, renderer, colors[3]);
    updateTextureText(tokenseatenTexture, "Chicken Eaten: 0", font, renderer, colors[8]);
    updateTextureText(pauseTexture, "GAME PAUSED. Press - to change game.", font, renderer, colors[8]);

    SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseBounds.w, &pauseBounds.h);
    pauseBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    pauseBounds.y = 200;

    // Load sound and music
    sound = loadSound("sounds/pop1.wav");
    music = loadMusic("music/background.ogg");

    Mix_PlayMusic(music, -1); // Play background music in loop

    // Timing variables
    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    // ------------------ MAIN LOOP ------------------
    while (isGameRunning && WHBProcIsRunning()) {
        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f; // Convert ms -> seconds
        previousFrameTime = currentFrameTime;

        handleEvents();          // Handle input events

        if (!isGamePaused) {     // Only update game logic if not paused
            update(deltaTime);
        }

        render();                // Draw everything
    }

    // ------------------ CLEANUP ------------------
    Mix_FreeMusic(music);
    Mix_FreeChunk(sound);
    SDL_DestroyTexture(playerSprite.texture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    stopSDLSystems();
    romfsExit();
    WHBProcShutdown();

    return 0; // Exit program
}
