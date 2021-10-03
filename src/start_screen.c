#include <stdio.h>
#include <string.h>
#include <math.h>

#include "raylib.h"

#include "tetramino.h"

#include "const.h"
#include "start_screen.h"
#include "game_screen.h"

static Rectangle stars[50];

static ActiveTetramino center;
static ActiveTetramino piece;
static Camera2D pieceCam;
static const float dist_scale = 6.5 * pow(10, 8);

static void DrawStars() {
    for (size_t i = 0; i < ARR_SIZE(stars); i++)
    {
        DrawCircle(stars[i].x, stars[i].y, stars[i].width, RAYWHITE);
    }
}

void start_init() {
    center.pos.x = SCREEN_WIDTH / 2;
    center.pos.y = SCREEN_HEIGHT / 2;
    center.block = &O_Block;

    memset(&stars, 0, sizeof(stars));
    for (size_t i = 0; i < ARR_SIZE(stars); i++)
    {
        stars[i].x = GetRandomValue(-SCREEN_WIDTH / 2, SCREEN_WIDTH * 1.5);
        stars[i].y = GetRandomValue(-SCREEN_HEIGHT / 2, SCREEN_HEIGHT * 1.5);
        stars[i].width = GetRandomValue(1, 3);
    }
}

screen_t start_update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        printf("enter pressed!\n");
        return game_screen;
    }
    
    return start_screen;
}

void start_draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawStars();
    draw_tetramino(center);
    EndDrawing();
}

void start_close() {

}

screen_t start_screen = {
    .name = 'STRT',
    .init = start_init,
    .update = start_update,
    .draw = start_draw,
    .close = start_close
};
