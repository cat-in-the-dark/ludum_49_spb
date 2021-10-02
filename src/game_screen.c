#include <stdio.h>
#include <string.h>
#include <math.h>

#include "raylib.h"

#include "const.h"
#include "game_screen.h"

typedef struct {
    float value;
    float speed;
} SpVector2;

typedef struct {
    SpVector2 distance;
    SpVector2 angle;
} PlanetState;

#define TILES_X 10
#define TILES_Y 10
#define TILE_W 10 // px
#define TILE_H 10
#define BLOCK_SIZE 4


static Color tilemap[TILES_Y][TILES_X] = {0};

typedef struct {
    Color color;
    int data[4][4][4];
} Tetramino;

typedef struct {
    int rot_index;
    Vector2 pos;
    Tetramino* block;
} ActiveTetramino;

static Tetramino I_Block = {
    .color = SKYBLUE,
    .data = {
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }, {
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 0}
    }, {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0}
    }, {
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    }}
};

static Tetramino J_Block = {
    .color = BLUE,
    .data = {
    {
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    }, {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    }, {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 1, 0}
    }, {
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0}
    }}
};

static Tetramino L_Block = {
    .color = ORANGE,
    .data = {
    {
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    }, {
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0}
    }, {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 0}
    }, {
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    }}
};

static Tetramino* Blocks[] = {&I_Block, &L_Block, &J_Block};

static const char* text = "Game screen!";
static Vector2 text_size;
static Vector2 star_pos;
static Vector2 planet_pos;
static Vector2 planet_speed;

static float star_mass;
static float gravity_const;
static float delta_time;

// delta time calculations
static float maxDeltaTime;
static float minDeltaTime;
static float maxDist;
static float minDist;

static float dist_scale;
static ActiveTetramino active_tetramino;

static const int centerX = SCREEN_WIDTH / 2;
static const int centerY = SCREEN_HEIGHT / 2;
static const int tileMapWidth = TILES_X * TILE_W;
static const int tileMapHeight = TILES_Y * TILE_H;
static const int tileMapPosX = centerX - tileMapWidth / 2;
static const int tileMapPosY = centerY - tileMapHeight / 2;

static PlanetState planet_state = {0};

float dist(Vector2 v1, Vector2 v2) {
    Vector2 dist = {.x = v1.x - v2.x, .y = v1.y - v2.y};
    return sqrt(dist.x * dist.x + dist.y * dist.y);
}

bool IsBlank(Color color) {
    return *(unsigned int*)&color == 0;
}

float distance_acceleration(PlanetState state) {
//   return state.distance.value * Math.pow(state.angle.speed, 2) -
    // (constants.gravitationalConstant * state.massOfTheSunKg) / Math.pow(state.distance.value, 2);
    return state.distance.value * pow(state.angle.speed, 2) -
    (gravity_const * star_mass) / pow(state.distance.value, 2);
}

float angle_acceleratioin(PlanetState state) {
    return -2.0f * state.distance.speed * state.angle.speed / state.distance.value;
}

float new_value(float current_value, float delta_time, float derivative) {
    return current_value + delta_time * derivative;
}

void draw_tilemap() {
    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            if (IsBlank(tilemap[i][j]))
            {
                continue;
            }

            int posX = (i - TILES_X / 2) * TILE_W + SCREEN_WIDTH / 2;
            int posY = (j - TILES_Y / 2) * TILE_H + SCREEN_HEIGHT / 2;
            DrawRectangleLines(posX, posY, TILE_W, TILE_H, tilemap[i][j]);
        }
    }
}

void ResetPlanetState() {
    memset(&planet_state, 0, sizeof(planet_state));
    planet_state.distance.value = 1.496 * pow(10, 11);
    // 3.5 * 10^8
    planet_state.angle.speed = 1.990986 * pow(10, -7);
}

void updateDeltaTime() {
    float dDeltaTime = maxDeltaTime - minDeltaTime;
    float dDist = maxDist - minDist;
    float targetDist = planet_state.distance.value;
    if (targetDist < minDist) {
        targetDist = minDist;
    } else if (targetDist > maxDist) {
        targetDist = maxDist;
    }

    delta_time = minDeltaTime + (dDeltaTime / dDist) * (targetDist - minDist);
}

void PlaceTetramino() {
    float deltaX = active_tetramino.pos.x - tileMapPosX;
    float deltaY = active_tetramino.pos.y - tileMapPosY;
    int ix = (int) round(deltaX / TILE_W);
    int iy = (int) round(deltaY / TILE_H);
    
    // TODO: Check bounds!
    int (*data)[4] = active_tetramino.block->data[active_tetramino.rot_index];
    for (size_t y = 0; y < BLOCK_SIZE; y++)
    {
        for (size_t x = 0; x < BLOCK_SIZE; x++)
        {
            if (data[y][x] == 0) {
                continue;
            }

            printf("%lu %lu Add\n", ix + x, iy + y);
            if (!IsBlank(tilemap[ix + x][iy + y])) {
                printf("%lu %lu is already taken!\n", ix + x, iy + y);
            }

            tilemap[ix + x][iy + y] = active_tetramino.block->color;
        }
    }

    ResetPlanetState();
    // TODO: 7!
    // TODO TODO: Blocks pool!
    active_tetramino.block = Blocks[GetRandomValue(0, 2)];
}

void intersect_tiles(ActiveTetramino block) {
    Rectangle tileMapRect = {
        .x = tileMapPosX,
        .y = tileMapPosY,
        .width = tileMapWidth,
        .height = tileMapHeight
    };

    Rectangle blockRect = {
        .x = block.pos.x,
        .y = block.pos.y,
        .width = TILE_W * BLOCK_SIZE,
        .height = TILE_H * BLOCK_SIZE
    };

    if (CheckCollisionRecs(tileMapRect, blockRect)) {
        printf("In tilemap!\n");
        for (size_t i = 0; i < TILES_X; i++)
        {
            for (size_t j = 0; j < TILES_Y; j++)
            {
                if (IsBlank(tilemap[i][j])) {
                    continue;
                }

                int posX = (i - TILES_X / 2) * TILE_W + centerX;
                int posY = (j - TILES_Y / 2) * TILE_H + centerY;
                Rectangle tileRect = {
                    .x = posX,
                    .y = posY,
                    .width = TILE_W,
                    .height = TILE_H
                };

                if (CheckCollisionRecs(tileRect, blockRect))
                {
                    printf("Hit tile %lu %lu\n", i, j);
                    int rot_index = block.rot_index;
                    int (*data)[4] = block.block->data[rot_index];
                    Rectangle targetRect = {.x = 0, .y = 0, .width = TILE_W, .height = TILE_H};
                    for (size_t y = 0; y < BLOCK_SIZE; y++)
                    {
                        for (size_t x = 0; x < BLOCK_SIZE; x++)
                        {
                            if (data[y][x] == 0) {
                                continue;
                            }

                            targetRect.x = block.pos.x + x * TILE_W;
                            targetRect.y = block.pos.y + y * TILE_H;

                            if (CheckCollisionRecs(tileRect, targetRect)) {
                                printf("Collision!\n");
                                DrawRectangleRec(targetRect, block.block->color);
                                DrawRectangleRec(tileRect, tilemap[i][j]);
                                PlaceTetramino();
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void draw_tetramino(ActiveTetramino tetramino) {
    int rot_index = tetramino.rot_index;
    int (*block)[4] = tetramino.block->data[rot_index];
    for (size_t i = 0; i < BLOCK_SIZE; i++)
    {
        for (size_t j = 0; j < BLOCK_SIZE; j++)
        {
            if (block[i][j] == 0) {
                continue;
            }

            DrawRectangleLines(tetramino.pos.x + j * TILE_W, tetramino.pos.y + i * TILE_H, 
                TILE_W, TILE_H, tetramino.block->color);
        }
    }
}

void game_init() {
    star_mass = 1.98855 * pow(10, 30);
    gravity_const = 6.67408 * pow(10, -11);
    delta_time = 3600 * 24;
    dist_scale = 6.5 * pow(10, 8);

    active_tetramino.block = &L_Block;
    active_tetramino.rot_index = 0;

    maxDeltaTime = 3600 * 24;
    minDeltaTime = 3600;
    maxDist = 1.496 * pow(10, 11);
    minDist = 3.5 * pow(10, 8);

    text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
    ResetPlanetState();

    star_pos.x = SCREEN_WIDTH / 2;
    star_pos.y = SCREEN_HEIGHT / 2;

    tilemap[4][4] = RED;
    tilemap[4][5] = GREEN;
    tilemap[5][4] = YELLOW;
    tilemap[5][5] = BLUE;

    printf("%s called\n", __FUNCTION__);
}

screen_t game_update() {
    float dist_acc = distance_acceleration(planet_state);
    planet_state.distance.speed = new_value(planet_state.distance.speed, delta_time, dist_acc);
    planet_state.distance.value = new_value(planet_state.distance.value, delta_time, planet_state.distance.speed);

    float angle_acc = angle_acceleratioin(planet_state);
    planet_state.angle.speed = new_value(planet_state.angle.speed, delta_time, angle_acc);
    planet_state.angle.value = new_value(planet_state.angle.value, delta_time, planet_state.angle.speed);

    if (IsKeyDown(KEY_UP)) {
        planet_state.angle.speed += pow(10, -9);
    }
    if (IsKeyDown(KEY_DOWN)) {
        planet_state.angle.speed -= pow(10, -9);
    }
    if (IsKeyDown(KEY_LEFT)) {
        planet_state.distance.speed += 50;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        planet_state.distance.speed -= 50;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        active_tetramino.rot_index = (active_tetramino.rot_index + 3) % 4;  // same as -1 % 4
    } else if (IsKeyPressed(KEY_RIGHT)) {
        active_tetramino.rot_index = (active_tetramino.rot_index + 1) % 4;
    }

    printf("%f %f %f %.15f %f\n", planet_state.distance.value, planet_state.distance.speed, planet_state.angle.value, planet_state.angle.speed, delta_time);

    float dist_to_scale = planet_state.distance.value / dist_scale;
    active_tetramino.pos.x = star_pos.x + dist_to_scale * cos(planet_state.angle.value);
    active_tetramino.pos.y = star_pos.y + dist_to_scale * sin(planet_state.angle.value);

    updateDeltaTime();

    intersect_tiles(active_tetramino);

    return game_screen;
}

void game_draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_tetramino(active_tetramino);
    draw_tilemap();

    EndDrawing();
    // DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, text_size.y + 10, 20, BLACK);
}

void game_close() {
    printf("%s called\n", __FUNCTION__);
}

screen_t game_screen = {
    .name = 'GAME',
    .init = game_init,
    .update = game_update,
    .draw = game_draw,
    .close = game_close
};
