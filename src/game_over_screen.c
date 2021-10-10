#include <stdio.h>

#include "game_over_screen.h"
#include "title_screen.h"
#include "game_screen.h"
#include "const.h"

#include "raylib.h"

extern int gamePoints;
static const char* text = "GAME OVER!";
static Vector2 text_size;

void go_init() {
    text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
}

screen_t go_update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        return game_screen;
    }
    
    return game_over_screen;
}

void go_draw() {
    ClearBackground(BLACK);
    DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, text_size.y + 10, 20, LIGHTGRAY);

    char points_text[64] = {0};
    snprintf(points_text, 63, "Score: %d", gamePoints);
    DrawText(points_text, 20, 20, 20, LIGHTGRAY);
}

void go_close() {

}

screen_t game_over_screen = {
    .name = 'GOVR',
    .init = go_init,
    .update = go_update,
    .draw = go_draw,
    .close = go_close
};
