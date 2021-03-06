#include <stdio.h>
#include <stdlib.h>
#include "include/raylib.h"
#include "include/raymath.h"

typedef struct Window
{
    unsigned int width;
    unsigned int height;
} Window;

// TODO: Move to state machine representation
typedef struct Player
{
    Rectangle rect;
    Color debugColor;
    Vector2 velocity;
    float acceleration;
    float jumpStrength;
    float maxBoost;
    float boostCharge;
    float boostStrength;
    float mass;
    int direction;
    int isMoving;
    int currentFrame;
    float timeSinceLastFrame;
} Player;

typedef struct RectangleEnv
{
    Rectangle rect;
    Color color;
    unsigned int state;
} RectangleEnv;

void printVec2(Vector2 rec);
void printRec(Rectangle rec);
Vector2 getTarget(Camera2D camera, Player player);
unsigned int checkUnsignedIntBit(unsigned int item, unsigned int n);
void updatePlayer(Player *player,
                  const RectangleEnv elements[],
                  int elementsSize,
                  float deltaTime);

const float GRAVITY = -9.8;
const int COLLISION_ALLOWANCE = 5;
const double PHYSICS_DELTA = 1.0 / 128.0;

bool isChangingFrames = false;
bool goalReached = false;
bool isDebugging = false;
bool resetGame = true;
int maxFPS = 144;

int main(void)
{
    // Initializing variables
    const Window window = {1280, 720};
    Color backgroundColor = BLACK;

    if (!isChangingFrames) { SetConfigFlags(FLAG_VSYNC_HINT); }
    InitWindow(window.width, window.height, "Raylib Testing");

    char winMessage[] = "You Win!";
    const int winMessageFontSize = 72;
    Vector2 winMessageSize = MeasureTextEx(
        GetFontDefault(),
        winMessage,
        winMessageFontSize,
        GetFontDefault().baseSize
    );

    Image skeletonImage = LoadImage("resources/skeleton.png");
    ImageResize(&skeletonImage, 500, 250);
    UnloadImage(skeletonImage);

    const Texture2D skeletonSpritesheet = LoadTextureFromImage(skeletonImage);
    const Texture2D backgroundTexture = LoadTexture("./resources/space.png");

    const int skeletonWidth = skeletonSpritesheet.width / 10;
    const int skeletonHeight = skeletonSpritesheet.height / 5;

    Player defaultPlayer = {
        .rect = {0, 0, 30, 50},
        .debugColor = (Color){255, 0, 0, 100},
        .velocity = {0, 0},
        .acceleration = 300,
        .jumpStrength = 3.5,
        .maxBoost = 100,
        .boostCharge = 0,
        .boostStrength = 2,
        .mass = 74,
        .direction = 1,
        .currentFrame = 0,
    };

    Player player;
    player = defaultPlayer;

    RectangleEnv defaultElements[] = {
        {{-10000, window.height * 2, 20000, 100}, GRAY, 1},
        {{0, window.height / 2, window.width, 100}, GRAY, 1},
        {{500, 0, 100, window.height}, GRAY, 1},
        {{450, 300, 50, 10}, GRAY, 1},
        {{200, 250, 50, 10}, GRAY, 1},
        {{195, 205, 10, 50}, GRAY, 1},
        {{300, 160, 50, 10}, GRAY, 1},
        {{100, 100, 100, 10}, GRAY, 1},
        {{450, 50, 50, 10}, GRAY, 1},

        {{525, -100, 50, 50}, GREEN, 2},
    };
    const int defaultElementsSize =
        sizeof(defaultElements) / sizeof(defaultElements[0]);

    RectangleEnv elements[255];
    int elementsSize = sizeof(elements) / sizeof(elements[0]);

    Camera2D camera = {0};
    camera.target = getTarget(camera, player);
    camera.offset = (Vector2){window.width / 2.0f, window.height / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    double physicsTotalTimeElapsed = 0.0;
    double frameTotalTitleElapsed = 0.0;
    double physicsTimeToCatchUp = 0.0;

    // Main game loop
    while (!WindowShouldClose())
    {
        // Timing Logic (Fixed Physics Update with varied rendering FPS)
        float frameDeltaTime = GetFrameTime();
        physicsTimeToCatchUp += frameDeltaTime;
        frameTotalTitleElapsed += frameDeltaTime;
        player.timeSinceLastFrame += frameDeltaTime;
        while (physicsTimeToCatchUp >= PHYSICS_DELTA)
        {
            camera.target = getTarget(camera, player);
            updatePlayer(&player, elements, elementsSize, PHYSICS_DELTA);

            physicsTimeToCatchUp -= PHYSICS_DELTA;
            physicsTotalTimeElapsed += PHYSICS_DELTA;
        }

        // Go back to original game state when resetting
        if (resetGame)
        {
            isChangingFrames = false;
            goalReached = false;
            maxFPS = 144;

            player = defaultPlayer;
            for (int i = 0; i < defaultElementsSize; i++)
                elements[i] = *(defaultElements + i);
            for (int i = defaultElementsSize; i < elementsSize; i++)
                elements[i] = (RectangleEnv){0};

            resetGame = false;
        }

        // Debugging
        if (IsKeyPressed(KEY_R)) { resetGame = true; }
        if (IsKeyPressed(KEY_F3)) { isDebugging = !isDebugging; }
        if (isChangingFrames) {
            SetTargetFPS(maxFPS);
            if (IsKeyPressed(KEY_EQUAL)) { maxFPS += 20; }
            if (IsKeyPressed(KEY_MINUS)) { maxFPS -= 20; }
        }

        BeginDrawing();
        {
            ClearBackground(backgroundColor);
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            BeginMode2D(camera);
            {
                // Draw Environment
                for (int i = 0; i < elementsSize; i++)
                {
                    DrawRectangleRec(elements[i].rect, elements[i].color);
                }

                // Draw Player
                DrawTextureRec(
                    skeletonSpritesheet,
                    (Rectangle)
                    {
                        skeletonWidth * player.currentFrame,
                        skeletonHeight * 2,
                        skeletonWidth * (player.direction ? 1 : -1),
                        skeletonHeight
                    },
                    (Vector2)
                    {
                        player.rect.x + (player.rect.width - skeletonWidth) / 2,
                        player.rect.y
                    },
                    WHITE);
                // Player Hitbox
                if (isDebugging)
                    DrawRectangleRec(player.rect, player.debugColor);
            }
            EndMode2D();

            // Draw the win message
            if (goalReached)
            {
                DrawText(winMessage,
                        (window.width - winMessageSize.x) / 2,
                        (window.height - winMessageSize.y) / 2,
                        winMessageFontSize, GREEN);
            }


            // Boost Indicator
            char boostChargeText[32];
            sprintf(boostChargeText, "Boost Fuel: %i/%i",
                    (int)player.boostCharge, (int)player.maxBoost);
            DrawText(boostChargeText, 25, window.height - 50, 25, BLUE);

            // More Debugging
            if (isDebugging) DrawFPS(0, 0);
        }
        EndDrawing();
    }

    UnloadTexture(backgroundTexture);
    UnloadTexture(skeletonSpritesheet);
    CloseWindow();

    return EXIT_SUCCESS;
}

// Main game logic
void updatePlayer(
    Player *player,
    const RectangleEnv elements[], int elementsSize,
    float deltaTime)
{
    bool isOnGround = false;
    int hasHitWall = 0;

    int pX = player->rect.x, pY = player->rect.y,
        pH = player->rect.height, pW = player->rect.width;
    for (int i = 0; i < elementsSize; i++)
    {
        if (checkUnsignedIntBit(elements[i].state, 1)
         && CheckCollisionRecs(player->rect, elements[i].rect))
        {
            goalReached = true;
        }

        // Check for collidable flag (first bit)
        if (!checkUnsignedIntBit(elements[i].state, 0)) continue;

        const int eX = elements[i].rect.x, eY = elements[i].rect.y,
                  eW = elements[i].rect.width, eH = elements[i].rect.height;
        if (pY + pH > eY + COLLISION_ALLOWANCE
         && pY < eY + eH - COLLISION_ALLOWANCE)
        {
            if (pX + pW > eX + eW && pX < eX + eW)
            {
                player->rect.x = eX + eW;
                hasHitWall = 1;
                pX = eX + eW;
                player->velocity.x = 0;
            }
            if (pX < eX && pX + pW > eX)
            {
                player->rect.x = eX - pW;
                hasHitWall = 1;
                pX = eX - pW;
                player->velocity.x = 0;
            }
        }
        if (pX + pW > eX + COLLISION_ALLOWANCE
         && pX < eX + eW - COLLISION_ALLOWANCE)
        {
            if (pY + pH > eY + eH && pY < eY + eH)
            {
                player->rect.y = eY + eH;
                pY = eY + eH;
                player->velocity.y = 0;
            }
            if (pY <= eY && pY + pH >= eY)
            {
                player->rect.y = eY - pH;
                pY = eY - pH;
                isOnGround = true;
                player->velocity.y = 0;
            }
        }
    }

    player->velocity.x *= (isOnGround ? 0.50 : 0.40) * deltaTime;
    player-> isMoving = false;

    if (isOnGround)
        player->boostCharge += 40 * deltaTime;
    if (player->boostCharge > player->maxBoost)
        player->boostCharge = player->maxBoost;
    if (!isOnGround)
        player->velocity.y -= GRAVITY * deltaTime;
    if (IsKeyDown(KEY_D)) {
        if (hasHitWall <= 0)
            player->velocity.x += player->acceleration * deltaTime;
        player->direction = 1;
        player->isMoving = true;
    }
    if (IsKeyDown(KEY_A)) {
        if (hasHitWall >= 0)
            player->velocity.x -= player->acceleration * deltaTime;
        player->direction = 0;
        player->isMoving = true;
    }
    if (IsKeyDown(KEY_W) && isOnGround)
        player->velocity.y -= player->jumpStrength;
    if (IsKeyDown(KEY_SPACE)
     && !isOnGround
     && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
     && player->boostCharge > 0)
    {
        player->boostCharge -= 100 * deltaTime;
        if (player->boostCharge < 0)
            player->boostCharge = 0;
        player->velocity.x *= player->boostStrength;
    }

    player->rect.x += player->velocity.x;
    player->rect.y += player->velocity.y;

    // Increment player animation frames
    if (player->isMoving && player->timeSinceLastFrame >= (1.0 / 30.0))
    {
            player->currentFrame = (player->currentFrame + 1) % 10;
            player->timeSinceLastFrame = 0.0;
    }
    if (!player->isMoving)
    {
        player->currentFrame = 0;
        player->timeSinceLastFrame = 0.0;
    }
}

// HELPER FUNCTIONS

unsigned int checkUnsignedIntBit(unsigned int item, unsigned int n)
{
    return item & (1 << n);
}

void printVec2(Vector2 vec) {
    printf("(%f, %f)\n", vec.x, vec.y);
}

void printRec(Rectangle rec)
{
    printf("(x: %f, y: %f, width: %f, height: %f)\n",
           rec.x, rec.y, rec.width, rec.height);
}

Vector2 getTarget(Camera2D camera, Player player)
{
    Vector2 playerCenter = {
        player.rect.x + player.rect.width / 2,
        player.rect.y + player.rect.height / 2
    };
    Vector2 target = Vector2Lerp(camera.target, playerCenter, 1.0 / 20.0);
    return target;
}