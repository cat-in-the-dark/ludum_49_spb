#include "raylib.h"

#include "start_screen.h"
#include "game_screen.h"

void start_init() {

}

screen_t start_update() {
    return start_screen;
}

void start_draw() {

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
