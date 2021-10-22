#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

typedef struct Window {
    int width;
    int height;
} Window;

typedef struct Player {
    Rectangle rect;
    Color color;
    Vector2 velocity;
    float speed;
    float jumpStrength;
    float maxBoost;
    float boostCharge;
    float boostStrength;
    float mass;
} Player;

Vector2 getTarget(Camera2D camera, Player player);
void updootPlayer(Player *player, const Rectangle elements[], int elementsSize, float deltaTime);

const float GRAVITY = -9.8;
const int COLLISION_ALLOWANCE = 2;

int main(void)
{
    // Initializing variables
    const Window window = { 1280, 720 };
    Color backgroundColor = BLACK;

    Player player = {
        .rect = { 50, 50, 50, 50 },
        .color = RED,
        .velocity = { 0, 0 },
        .speed = 300, .jumpStrength = 500,
        .maxBoost = 100, .boostCharge = 0, .boostStrength = 2,
        .mass = 74
    };

    Rectangle elements[] = {
        { 0, window.height / 2, window.width, 100 },
        { window.width / 1.5, 0, 100, window.height }
    };
    const int elementsSize = sizeof(elements) / sizeof(elements[0]);

    Camera2D camera = { 0 };
    camera.target = getTarget(camera, player);
    camera.offset = (Vector2) { window.width / 2.0f, window.height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    InitWindow(window.width, window.height, "Raylib Testing");
    SetTargetFPS(144);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(backgroundColor);

            DrawFPS(0, 0);
            char boostChargeText[100];
            sprintf(boostChargeText, "Boost Fuel: %i/%i",
                (int) player.boostCharge, (int) player.maxBoost);
            DrawText(boostChargeText, 25, window.height - 50, 25, BLUE);

            const float deltaTime = GetFrameTime();

            updootPlayer(&player, elements, elementsSize, deltaTime);

            camera.target = getTarget(camera, player);
            BeginMode2D(camera);
            {
                for (int i = 0; i < elementsSize; i++) {
                    DrawRectangleRec(elements[i], GRAY);
                }
                DrawRectangleRec(player.rect, player.color);
            }
            EndMode2D();
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void updootPlayer(Player *player, const Rectangle elements[], int elementsSize, float deltaTime) {
    bool isOnGround = false;
    int hasHitWall = 0;

    const int pX = player->rect.x;
    const int pY = player->rect.y;
    const int pHeight = player->rect.height;
    const int pWidth = player->rect.width;
    for (int i = 0; i < elementsSize; i++) {
        const int elX = elements[i].x;
        const int elY = elements[i].y;
        const int elWidth = elements[i].width;
        const int elHeight = elements[i].height;

        if (pY + pHeight >= elY
            && pY <= elY + COLLISION_ALLOWANCE
            && pX + pWidth >= elX
            && pX < elX + elWidth
        ) {
            player->rect.y = elY - pHeight;
            player->velocity.y = 0;
            isOnGround = true;
        }
        if (pX + pWidth >= elX
            && pX <= elX + COLLISION_ALLOWANCE
            && ((pY > elY && pY < elY + elHeight)
                || (pY + pHeight > elY && pY + pHeight < elY + elHeight))
        ) {
            player->rect.x = elX - pWidth - 1;
            hasHitWall = 1;
        }
        if (pX <= elX + elWidth
            && pX + pWidth >= elX + elWidth - COLLISION_ALLOWANCE
            && ((pY > elY && pY < elY + elHeight)
                || (pY + pHeight > elY && pY + pHeight < elY + elHeight))
        ) {
            player->rect.x = elX + elWidth + 1;
            hasHitWall = -1;
        }
    }
    player->velocity.x *= isOnGround ? 0.50 : 0.30,
    player->velocity.y *= 0.98;

    if (isOnGround) player->boostCharge += 40 * deltaTime;
    if (player->boostCharge > player->maxBoost) player->boostCharge = player->maxBoost;
    if (!isOnGround) player->velocity.y -= GRAVITY * deltaTime;

    if (IsKeyDown(KEY_D) && hasHitWall <= 0) player->velocity.x += player->speed * deltaTime;
    if (IsKeyDown(KEY_A) && hasHitWall >= 0) player->velocity.x -= player->speed * deltaTime;
    if (IsKeyDown(KEY_W) && isOnGround) player->velocity.y -= player->jumpStrength * deltaTime;
    if (IsKeyDown(KEY_SPACE)
        && !isOnGround
        && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        && player->boostCharge > 0
    ) {
        player->boostCharge -= 100 * deltaTime;
        if (player->boostCharge < 0) player->boostCharge = 0;
        player->velocity.x *= player->boostStrength;
    }

    player->rect.x = (int) player->velocity.x + player->rect.x;
    player->rect.y = (int) player->velocity.y + player->rect.y;
}

Vector2 getTarget(Camera2D camera, Player player) {
    Vector2 playerCenter = {
        player.rect.x + player.rect.width / 2,
        player.rect.y + player.rect.width / 2
    };
    Vector2 target = Vector2Lerp(camera.target, playerCenter, 1.0 / 20.0);
    return target;
}
