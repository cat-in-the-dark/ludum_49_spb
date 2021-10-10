#include "raylib.h"
#include <stdio.h>

#include "const.h"
#include "title_screen.h"
#include "game_screen.h"
#include "start_screen.h"

static const char* text = "CAT IN THE DARK ";
static Vector2 text_size;

void title_init() {
    text_size = MeasureTextEx(GetFontDefault(), text, 32, 1);
}

screen_t title_update() {
    if (IsKeyPressed(KEY_ENTER))
    {
        printf("enter pressed!\n");
        return start_screen;
    }
    
    return title_screen;
}

void title_draw() {
    ClearBackground(BLACK);

    int scale = 20;
    int size = 11 * scale;
    int startX = SCREEN_WIDTH / 2 - size / 2, startY = SCREEN_HEIGHT / 2 - size / 2 - 50;
    Rectangle rect = {startX, startY, size, size};
    DrawRectangleLinesEx(rect, 2, RAYWHITE);
    DrawRectangle(startX + 1 * scale, startY + 1 * scale, scale, scale, RAYWHITE);
    DrawRectangle(startX + 4 * scale, startY + 1 * scale, scale, scale, RAYWHITE);
    DrawRectangle(startX + 1 * scale, startY + 2 * scale, 4 * scale, 3 * scale, RAYWHITE);
    DrawRectangle(startX + 1 * scale, startY + 5 * scale, 3 * scale, 6 * scale, RAYWHITE);
    DrawRectangle(startX + 4 * scale, startY + 6 * scale, 2 * scale, scale, RAYWHITE);

    DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, 360, 32, LIGHTGRAY);
    DrawText("Press [Enter]", SCREEN_WIDTH / 2 - text_size.x / 2, 400, 20, LIGHTGRAY);
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
