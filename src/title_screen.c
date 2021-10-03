#include "raylib.h"
#include <stdio.h>

#include "const.h"
#include "title_screen.h"
#include "game_screen.h"

static const char* text = "Title screen!";
static Vector2 text_size;

void title_init() {
    text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
}

screen_t title_update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        printf("enter pressed!\n");
        return game_screen;
    }
    
    return title_screen;
}

void title_draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, text_size.y + 10, 20, BLACK);
    EndDrawing();
}

void title_close() {
    printf("%s called\n", __FUNCTION__);
}

screen_t title_screen = {
    .name = 'TITL',
    .init = title_init,
    .update = title_update,
    .draw = title_draw,
    .close = title_close
};
