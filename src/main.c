#include <math.h>
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
RenderTexture2D target;

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
    float scale = fmin((float)GetScreenWidth()/SCREEN_WIDTH, (float)GetScreenHeight()/SCREEN_HEIGHT);
    BeginTextureMode(target);
    current_screen.draw();
    EndTextureMode();
    BeginDrawing();
    ClearBackground(BLACK);     // Clear screen background

    // Draw render texture to screen, properly scaled
    DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                   (Rectangle){ (GetScreenWidth() - ((float)SCREEN_WIDTH*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_HEIGHT*scale))*0.5f,
                   (float)SCREEN_WIDTH*scale, (float)SCREEN_HEIGHT*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

int main(int argc, char const *argv[])
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window title");

    target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

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
