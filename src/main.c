#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "raylib.h"

#include "const.h"
#include "screen.h"
#include "title_screen.h"
#include "game_screen.h"

screen_t current_screen;

void change_screen(screen_t old, screen_t new) {
    old.close();
    new.init();
    current_screen = new;
}

void update_screen() {
    screen_t new_screen = current_screen.update();
    if (new_screen.name != current_screen.name) {
        printf("Change screen!\n");
        change_screen(current_screen, new_screen);
    }
}

void UpdateDrawFrame() {
    update_screen();
    current_screen.draw();
}

int main(int argc, char const *argv[])
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");

    current_screen = title_screen;
    current_screen.init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    current_screen.close();
    CloseWindow();

    return 0;
}
