#include <stdio.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#include "tetramino.h"

#include "const.h"
#include "start_screen.h"
#include "game_screen.h"
#include "orbital.h"

static Rectangle stars[50];

static ActiveTetramino center;
static ActiveTetramino piece;
static Camera2D pieceCam;
static float dist_scale;

// orbital stuff
static PlanetState planetState;
static float starMass;
static float deltaTime;
static Vector2 starPos;

static Vector2 oldPos;
static Vector2 newTarget;
static Vector2 oldTarget;
static float progress;
static bool moveCam;

static int alpha;

static void DrawStars() {
    for (size_t i = 0; i < ARR_SIZE(stars); i++)
    {
        DrawCircle(stars[i].x, stars[i].y, stars[i].width, RAYWHITE);
    }
}

static void ResetPlanetState() {
    memset(&planetState, 0, sizeof(planetState));
    planetState.distance.value = 2.496 * pow(10, 11);
    // TODO: create more interesting orbit
    planetState.angle.speed = -0.590986 * pow(10, -7);
    planetState.angle.value = 3 * PI / 2;
}

void start_init() {
    starMass = 1.98855 * pow(10, 30);
    deltaTime = 3600 * 24;
    dist_scale = 6.5 * pow(10, 8);
    starPos.x = SCREEN_WIDTH / 2;
    starPos.y = SCREEN_HEIGHT / 2;

    moveCam = false;
    progress = 0;
    alpha = 255;

    memset(&planetState, 0, sizeof(PlanetState));
    ResetPlanetState();

    center.pos.x = SCREEN_WIDTH / 2;
    center.pos.y = SCREEN_HEIGHT / 2;
    center.block = &O_Block;

    piece.block = GetRandomBlock();

    pieceCam.offset = starPos;
    pieceCam.zoom = 1.0f;

    memset(&stars, 0, sizeof(stars));
    for (size_t i = 0; i < ARR_SIZE(stars); i++)
    {
        stars[i].x = GetRandomValue(-SCREEN_WIDTH / 2, SCREEN_WIDTH * 1.5);
        stars[i].y = GetRandomValue(-SCREEN_HEIGHT / 2, SCREEN_HEIGHT * 1.5);
        stars[i].width = GetRandomValue(1, 3);
    }
}

screen_t start_update() {
    UpdatePlanetState(&planetState, deltaTime, starMass);

    Vector2 piecePos = StateToCoords(planetState, dist_scale, starPos);
    piece.pos.x = piecePos.x;
    piece.pos.y = piecePos.y;

    if (moveCam) {
        pieceCam.target = Vector2Lerp(oldTarget, newTarget, progress);
        progress += 0.03;
        if (progress >= 1.0) {
            progress = 1.0;
        }
    } else {
        pieceCam.target = piecePos;
    }
    
    if (!moveCam && Vector2Distance(piece.pos, starPos) > Vector2Distance(oldPos, starPos)) {
        newTarget = starPos;
        oldTarget = piecePos;
        moveCam = true;
    }


    oldPos = piece.pos;

    if (IsKeyPressed(KEY_ENTER))
    {
        printf("enter pressed!\n");
        return game_screen;
    }
    
    return start_screen;
}

void start_draw() {
    ClearBackground(BLACK);

    BeginMode2D(pieceCam);
    DrawStars();
    draw_tetramino(center);
    draw_tetramino(piece);

    DrawText("ORBITRIS", starPos.x, starPos.y + 20, 64, LIGHTGRAY);
    DrawText("Press [Enter]", starPos.x, starPos.y + 100, 20, LIGHTGRAY);
    EndMode2D();

    Color fade = BLACK;
    if (alpha >= 5) {
        alpha -= 3;
    }
    fade.a = alpha;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fade);
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
