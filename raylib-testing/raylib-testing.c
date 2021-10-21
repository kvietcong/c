#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

typedef struct Window {
    int width;
    int height;
} Window;

typedef struct DrawableRectangle {
    int x;
    int y;
    int width;
    int height;
    Color color;
} DrawableRectangle;

typedef struct Player {
    DrawableRectangle body;
    Vector2 velocity;
    float speed;
    float maxBoost;
    float boostCharge;
} Player;

void drawRectangle(DrawableRectangle rectangle) {
    DrawRectangle(
        rectangle.x, rectangle.y,
        rectangle.width, rectangle.height,
        rectangle.color
    );
}

#define GRAVITY -9.8f

void updootPlayer(Player *player, DrawableRectangle elements[], int elementsSize, float deltaTime) {
    bool hasBeenGrounded = false;
    int hasHitWall = 0;

    const int pX = player->body.x;
    const int pY = player->body.y;
    const int pHeight = player->body.height;
    const int pWidth = player->body.width;
    for (int i = 0; i < elementsSize; i++) {
        const int elX = elements[i].x;
        const int elY = elements[i].y;
        const int elWidth = elements[i].width;
        const int elHeight = elements[i].height;
        // printf(
        //     "Player: (x %i, y %i, xf %i, yf %i) Element %i: (x %i, y %i, xf %i, yf %i)\n",
        //     pX, pY, pXf, pYf,
        //     i, elX, elY, elX+elWidth, elY+elHeight
        // );
        if (pY + pHeight >= elY
            && pY <= elY + 5
            && pX + pWidth >= elX
            && pX < elX + elWidth
        ) {
            player->body.y = elY - pHeight;
            player->velocity.y = 0;
            hasBeenGrounded = true;
        }
        if (pX + pWidth >= elX
            && pX <= elX + 5
            && pY >= elY
            && pY + pHeight <= elY + elHeight
        ) {
            player->body.x = elX - pWidth - 1;
            hasHitWall = 1;
        }
        if (pX <= elX + elWidth
            && pX + pWidth >= elX + elWidth - 5
            && pY >= elY
            && pY + pHeight <= elY + elHeight
        ) {
            player->body.x = elX + elWidth + 1;
            hasHitWall = -1;
        }
    }

    player->velocity.x *= hasBeenGrounded ? 0.50 : 0.25,
    player->velocity.y *= 0.98;

    if (hasBeenGrounded) player->boostCharge += 20 * deltaTime;
    if (player->boostCharge > player->maxBoost) player->boostCharge = player->maxBoost;
    if (!hasBeenGrounded) player->velocity.y -= GRAVITY * deltaTime;

    if (IsKeyDown(KEY_D) && hasHitWall <= 0) player->velocity.x += player->speed * deltaTime;
    if (IsKeyDown(KEY_A) && hasHitWall >= 0) player->velocity.x -= player->speed * deltaTime;
    if (IsKeyDown(KEY_W) && hasBeenGrounded) player->velocity.y -= player->speed * deltaTime;
    if (IsKeyDown(KEY_SPACE)
        && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))
        && player->boostCharge > 0
    ) {
        player->boostCharge -= 100 * deltaTime;
        if (player->boostCharge < 0) player->boostCharge = 0;
        player->velocity.x *= 2;
    }

    player->body.x = (int) player->velocity.x + player->body.x;
    player->body.y = (int) player->velocity.y + player->body.y;
}

int main(void)
{
    const Window window = { 1280, 720 };

    Color backgroundColor = BLACK;

    Player player = { { 50, 50, 50, 50, RED }, { 0, 0 }, 300, 100, 0 };
    DrawableRectangle elements[] = {
        { 0, window.height / 2, window.width, 100, GRAY },
        { window.width / 1.5, 0, 100, window.height, GRAY }
    };
    const int elementsSize = sizeof(elements) / sizeof(elements[0]);

    InitWindow(window.width, window.height, "Raylib Testing");
    ClearBackground(backgroundColor);
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(backgroundColor);

            const float deltaTime = GetFrameTime();

            // player.velocity = getNewVelocity(player, elements, deltaTime);
            updootPlayer(&player, elements, elementsSize, deltaTime);
            // player = updatePlayer(player);

            for (int i = 0; i < elementsSize; i++) {
                drawRectangle(elements[i]);
            }
            drawRectangle(player.body);
            if (IsKeyPressed(KEY_ESCAPE)) CloseWindow();
            DrawFPS(0, 0);
            char boostChargeText[100];
            sprintf(boostChargeText, "Boost Fuel: %i/%i",
                (int) player.boostCharge, (int) player.maxBoost);
            DrawText(boostChargeText, 25, window.height - 50, 25, BLUE);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}