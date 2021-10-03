#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"
#include "raymath.h"

#include "const.h"
#include "game_screen.h"
#include "game_over_screen.h"

typedef struct {
    float value;
    float speed;
} SpVector2;

typedef struct {
    SpVector2 distance;
    SpVector2 angle;
} PlanetState;

#define TILES_X 20
#define TILES_Y 20
#define TILE_W 16 // px
#define TILE_H 16
#define ROW_LENGTH 8
#define DEATH_LENGTH 12
#define BLOCK_SIZE 4

// special alpha value to mark block to be deleted
#define FLAG_TO_DELETE 224
#define FLAG_OOB 223

#define ARR_SIZE(X) (sizeof(X) / sizeof(X[0]))

typedef struct {
    Color color;
    Vector2 pos;
    Vector2 speed;
} MovingPoint;

static Color tilemap[TILES_X][TILES_Y] = {0};
static MovingPoint explodeMap[TILES_X][TILES_Y] = {0};

typedef struct {
    Color color;
    Vector2 center; // in tiles;
    int data[4][4][4];
} Tetramino;

typedef struct {
    int rot_index;
    Vector2 pos;
    Tetramino* block;
    // to slide in place
    Vector2 oldPos;
    Vector2 targetPos;
    float progress;  // [0.0, 1.0]
} ActiveTetramino;

typedef struct {
    bool rows[TILES_Y];
    bool columns[TILES_X];
} TileDeleteInfo;

static Tetramino I_Block = {
    .color = SKYBLUE,
    .center = {.x = 2, .y = 2},
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
    .center = {.x = 1.5, .y = 2.5},
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
    .center = {.x = 1.5, .y = 2.5},
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

static Tetramino O_Block = {
    .color = YELLOW,
    .center = {.x = 2, .y = 2},
    .data = {
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }
    }
};

static Tetramino S_Block = {
    .color = GREEN,
    .center = {.x = 1.5, .y = 2.5},
    .data = {
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
        }, {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0}
        }, {
            {0, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0}
        }
    }
};

static Tetramino T_Block = {
    .color = PURPLE,
    .center = {.x = 1.5, .y = 2.5},
    .data = {
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0}
        }
    }
};

static Tetramino Z_Block = {
    .color = RED,
    .center = {.x = 1.5, .y = 2.5},
    .data = {
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0}
        }, {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0}
        }, {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {1, 0, 0, 0}
        }
    }
};

static Tetramino* Blocks[] = {&I_Block, &L_Block, &J_Block, &O_Block, &S_Block, &T_Block, &Z_Block};
// static Tetramino* Blocks[] = {&Z_Block};

static TileDeleteInfo tileDeleteInfo;
static float deleteProgress;

// game over visualization
static const float coloLerpSpeed = 0.02;
static const int gameOverProgressSpeed = 2;
static int gameOverProgress;
static const int gameOverValue = 255;
static const int tilemapAlpha = 100;
static const int gameOverAlphaThreshold = 5;
static bool explode;
static Color gameOverColor;

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
static ActiveTetramino next_tetramino;
static ActiveTetramino sliding_tetramino;

static const int centerX = SCREEN_WIDTH / 2;
static const int centerY = SCREEN_HEIGHT / 2;
static const int tileMapWidth = TILES_X * TILE_W;
static const int tileMapHeight = TILES_Y * TILE_H;
static const int tileMapPosX = centerX - tileMapWidth / 2;
static const int tileMapPosY = centerY - tileMapHeight / 2;

static const float progressSpeed = 0.05f;
static const float distThreshold = 0.0001f;

static PlanetState planet_state = {0};

static const float maxPieceDistance = SCREEN_HEIGHT * 2;
static const int newPieceTextFrames = 120;
static int newPieceTextCurrentFrame;

// camera
static Camera2D camera;
static float newZoom;
static const float zoomSpeed = 0.005;
static const int frameOffset = 20;  // px

static bool gameOver = false;
int gamePoints;

float dist(Vector2 v1, Vector2 v2) {
    Vector2 dist = {.x = v1.x - v2.x, .y = v1.y - v2.y};
    return sqrt(dist.x * dist.x + dist.y * dist.y);
}

bool IsBlank(Color color) {
    return *(unsigned int*)&color == 0;
}

float distance_acceleration(PlanetState state) {
    return state.distance.value * pow(state.angle.speed, 2) -
    (gravity_const * star_mass) / pow(state.distance.value, 2);
}

float angle_acceleratioin(PlanetState state) {
    return -2.0f * state.distance.speed * state.angle.speed / state.distance.value;
}

float new_value(float current_value, float delta_time, float derivative) {
    return current_value + delta_time * derivative;
}

void DeleteTilesForReal() {
    for (size_t i = 0; i < TILES_X; i++)
    {
        if (tileDeleteInfo.columns[i])
        {
            size_t di = 1, max_i = TILES_X;
            if (i < TILES_X / 2) {
                printf("Move right\n");
                di = -1;
                max_i = 0;
            } else {
                printf("Move left\n");
            }

            for (size_t i1 = i; i1 != max_i; i1 = i1 + di)
            {
                if ((di > 0 && i1 == TILES_X - 1) || (di < 0 && i1 == 0)) {
                    tileDeleteInfo.columns[i1] = false;
                } else {
                    tileDeleteInfo.columns[i1] = tileDeleteInfo.columns[i1 + di];
                }

                for (size_t j = 0; j < TILES_Y; j++)
                {
                    if ((di > 0 && i1 == TILES_X - 1) || (di < 0 && i1 == 0)) {
                        tilemap[i1][j] = BLANK;
                    } else {
                        tilemap[i1][j] = tilemap[i1 + di][j];
                    }
                }
            }

            // check same column again
            if (di > 0) {
                i = i - 1;
            }
        }
    }

    for (size_t j = 0; j < TILES_Y; j++)
    {
        if (tileDeleteInfo.rows[j]) {
            size_t dj = 1, max_j = TILES_Y;
            if (j < TILES_Y / 2) {
                printf("Move down\n");
                dj = -1;
                max_j = 0;
            } else {
                printf("Move up\n");
            }

            for (size_t j1 = j; j1 != max_j; j1 = j1 + dj)
            {
                if ((dj > 0 && j1 == TILES_Y - 1) || (dj < 0 && j1 == 0)) {
                    tileDeleteInfo.rows[j1] = false;
                } else {
                    tileDeleteInfo.rows[j1] = tileDeleteInfo.rows[j1 + dj];
                }

                for (size_t i = 0; i < TILES_X; i++)
                {
                    if ((dj > 0 && j1 == TILES_Y - 1) || (dj < 0 && j1 == 0)) {
                        tilemap[i][j1] = BLANK;
                    } else {
                        tilemap[i][j1] = tilemap[i][j1+dj];
                    }
                }
            }

            // check same row again
            if (dj > 0) {
                j = j -1;
            }
        }
    }
}

void draw_tilemap() {
    // draw tetirs boundaries
    Rectangle rect = {
        .width = ROW_LENGTH * TILE_W,
        .height = ROW_LENGTH * TILE_H
    };

    rect.x = centerX - rect.width / 2;
    rect.y = centerY - rect.height / 2;

    Color c = GRAY;
    c.a = 127;
    DrawRectangleLinesEx(rect, 2, c);

    rect.width = DEATH_LENGTH * TILE_W;
    rect.height = DEATH_LENGTH * TILE_H;
    rect.x = centerX - rect.width / 2;
    rect.y = centerY - rect.height / 2;
    c = RED;
    c.a = 127;
    DrawRectangleLinesEx(rect, 2, c);

    bool hasOOB = false;

    // draw tiles
    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            if (IsBlank(tilemap[i][j]))
            {
                continue;
            }

            int size = TILE_W;
            if (tilemap[i][j].a == FLAG_TO_DELETE) {
                size = deleteProgress;
            }

            int posX = (i - TILES_X / 2) * TILE_W + centerX + (TILE_W - size) / 2;
            int posY = (j - TILES_Y / 2) * TILE_H + centerY + (TILE_H - size) / 2;

            if (tilemap[i][j].a == FLAG_OOB) {
                hasOOB = true;
                posX += GetRandomValue(-2, 2);
                posY += GetRandomValue(-2, 2);
                tilemap[i][j].r = Lerp(tilemap[i][j].r, 255, coloLerpSpeed);
                tilemap[i][j].g = Lerp(tilemap[i][j].g, 0, coloLerpSpeed);
                tilemap[i][j].b = Lerp(tilemap[i][j].b, 0, coloLerpSpeed);
            }

            Color bg = tilemap[i][j];
            if (tilemap[i][j].a == FLAG_OOB) {
                bg.a = gameOverProgress;
            } else {
                bg.a = tilemapAlpha;
            }

            DrawRectangle(posX, posY, size, size, bg);
            DrawRectangleLines(posX, posY, size, size, tilemap[i][j]);
        }
    }

    if (deleteProgress > 0) {
        deleteProgress -= 1;
    } else if (deleteProgress == 0) {
        DeleteTilesForReal();
    }

    if (hasOOB && gameOverProgress < gameOverValue) {
        gameOverProgress += gameOverProgressSpeed;
    }
}

void ResetPlanetState() {
    memset(&planet_state, 0, sizeof(planet_state));
    planet_state.distance.value = 1.496 * pow(10, 11);
    planet_state.angle.speed = 1.990986 * pow(10, -7);
}

void GenerateNextTetramino() {
    // TODO: Blocks pool!
    if (next_tetramino.block == NULL) {
        active_tetramino.block = Blocks[GetRandomValue(0, ARR_SIZE(Blocks) - 1)];
    } else {
        active_tetramino.block = next_tetramino.block;
    }

    next_tetramino.block = Blocks[GetRandomValue(0, ARR_SIZE(Blocks) - 1)];
    active_tetramino.rot_index = 0;
}

float UpdateDeltaTime(PlanetState state) {
    float dDeltaTime = maxDeltaTime - minDeltaTime;
    float dDist = maxDist - minDist;
    float targetDist = state.distance.value;
    if (targetDist < minDist) {
        targetDist = minDist;
    } else if (targetDist > maxDist) {
        targetDist = maxDist;
    }

    return minDeltaTime + (dDeltaTime / dDist) * (targetDist - minDist);
}

Vector2 GetTilePos(int x, int y) {
    Vector2 result = {.x = 0, .y = 0};
    result.x = x * TILE_W + centerX - (TILE_W * TILES_X / 2);
    result.y = y * TILE_H + centerY - (TILE_H * TILES_Y / 2);
    return result;
}

void GetTetraminoTilemapPosCorner(ActiveTetramino block, int* x, int* y) {
    size_t idx = 0;
    float deltaX = block.pos.x - block.block->center.x * TILE_W - tileMapPosX;
    float deltaY = block.pos.y - block.block->center.y * TILE_H - tileMapPosY;
    int ix = (int) round(deltaX / TILE_W);
    int iy = (int) round(deltaY / TILE_H);

    *x = ix;
    *y = iy;
}

void GetTetraminoTilemapPos(ActiveTetramino block, int (*coords)[2] /* int[4][2] */) {
    size_t idx = 0;
    float deltaX = block.pos.x - block.block->center.x * TILE_W - tileMapPosX;
    float deltaY = block.pos.y - block.block->center.y * TILE_H - tileMapPosY;
    int ix = 0, iy = 0;
    GetTetraminoTilemapPosCorner(block, &ix, &iy);

    // TODO: Check bounds!
    int (*data)[BLOCK_SIZE] = block.block->data[block.rot_index];
    for (size_t y = 0; y < BLOCK_SIZE; y++)
    {
        for (size_t x = 0; x < BLOCK_SIZE; x++)
        {
            if (data[y][x] == 0) {
                continue;
            }

            coords[idx][0] = ix + x;
            coords[idx][1] = iy + y;
            idx++;
        }
    }
}

void CheckTilesOOB() {
    int startX = (TILES_X - DEATH_LENGTH) / 2, startY = (TILES_Y - DEATH_LENGTH) / 2;
    int endX = startX + DEATH_LENGTH, endY = startY + DEATH_LENGTH;
    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            if (i < startX || i >= endX || j < startY || j >= endY)
            {
                if(!IsBlank(tilemap[i][j])) {
                    printf("OOB tile at %lu %lu!\n", i, j);
                    tilemap[i][j].a = FLAG_OOB;
                    if (gameOverProgress == 0) {
                        gameOverProgress = tilemapAlpha;
                    }
                    // return true;
                }
            }
        }
    }

    // return false;
}

void CheckRows() {
    int hits = 0;

    for (size_t i = 0; i < TILES_X; i++)
    {
        size_t row_start = (TILES_Y - ROW_LENGTH) / 2;
        bool hit = true;
        for (size_t j = row_start; j < row_start + ROW_LENGTH; j++)
        {
            if (IsBlank(tilemap[i][j])) {
                hit = false;
            }
        }
        
        if (hit) {
            hits++;
            printf("Filled by Y at x = %lu\n", i);
            tileDeleteInfo.columns[i] = true;
            for (size_t j1 = 0; j1 < TILES_Y; j1++)
            {
               if (!IsBlank(tilemap[i][j1])) {
                   tilemap[i][j1].a = FLAG_TO_DELETE;
               }
            }
        }
    }

    for (size_t j = 0; j < TILES_Y; j++)
    {
        size_t col_start = (TILES_X - ROW_LENGTH) / 2;
        bool hit = true;
        for (size_t i = col_start; i < col_start + ROW_LENGTH; i++)
        {
            if (IsBlank(tilemap[i][j])) {
                hit = false;
            }
        }
        
        if (hit) {
            hits++;
            printf("Filled by X at y = %lu\n", j);
            tileDeleteInfo.rows[j] = true;
            for (size_t i1 = 0; i1 < TILES_X; i1++)
            {
                if (!IsBlank(tilemap[i1][j])) {
                    tilemap[i1][j].a = FLAG_TO_DELETE;
                }
            }
        }
    }

    if (hits > 0) {
        deleteProgress = TILE_W;
    }

    switch (hits)
    {
    case 1:
        gamePoints += 40;
        break;
    case 2:
        gamePoints += 100;
        break;
    case 3:
        gamePoints += 300;
        break;
    case 4:
        gamePoints += 1200;
        break;
    default:
        if (hits > 4) {
            // Is it even possible??
            gamePoints += 2400;
        }
        break;
    }
}

void PlaceTetramino(ActiveTetramino* block) {
    int coords[BLOCK_SIZE][2] = {};
    GetTetraminoTilemapPos(*block, coords);
    for (size_t i = 0; i < BLOCK_SIZE; i++)
    {
        printf("Add %d %d\n", coords[i][0], coords[i][1]);
        tilemap[coords[i][0]][coords[i][1]] = block->block->color;
    }

    CheckRows();
    // if (CheckTilesOOB()) {
    //     gameOver = true;
    // }
    CheckTilesOOB();
}

bool CanMove(ActiveTetramino* block, int dx, int dy) {
    int coords[BLOCK_SIZE][2] = {};
    GetTetraminoTilemapPos(*block, coords);
    for (size_t i = 0; i < BLOCK_SIZE; i++)
    {
        if (!IsBlank(tilemap[coords[i][0] + dx][coords[i][1] + dy]))
        {
            return false;
        }
    }
    
    return true;
}

void MoveSlidingTetramino(ActiveTetramino* block) {
    if (block->block == NULL) {
        return;
    }

    if (block->progress >= 1) {
        printf("Arrived at (%f %f), check next\n", block->targetPos.x, block->targetPos.y);
    } else {
        block->progress += progressSpeed;
        block->pos = Vector2Lerp(block->oldPos, block->targetPos, block->progress);
        printf("Sliding to (%f; %f)\n", block->pos.x, block->pos.y);
        return;
    }

    int threshold = TILE_W;
    float dx = centerX - block->pos.x;
    float dy = centerY - block->pos.y;

    int dirX = dx > 0 ? 1 : -1;
    int dirY = dy > 0 ? 1 : -1;

    if (abs(dx) < threshold) {
        dirX = 0;
    }

    if (abs(dy) < threshold) {
        dirY = 0;
    }

    if (dirX == 0 && dirY == 0) {
        printf("We're here!\n");
        PlaceTetramino(block);
        block->block = NULL;
        return;
    }

    // check larger distance first
    int move = 0;
    if (abs(dx) > abs(dy)) {
        if (dirX != 0 && CanMove(block, dirX, 0)) {
            printf("Can move %d on x\n", dirX);
            dirY = 0;
            move = 1;
        } else if (dirY != 0 && CanMove(block, 0, dirY)) {
            printf("Can move %d on y\n", dirY);
            dirX = 0;
            move = 1;
        }
    } else {
        if (dirY != 0 && CanMove(block, 0, dirY)) {
            printf("Can move %d on y\n", dirY);
            dirX = 0;
            move = 1;
        } else if (dirX != 0 && CanMove(block, dirX, 0)) {
            printf("Can move %d on x\n", dirX);
            dirY = 0;
            move = 1;
        }
    }

    if (!move) {
        printf("Can't move further!\n");
        PlaceTetramino(block);
        block->block = NULL;
        return;
    }

    int ix = 0, iy = 0;
    GetTetraminoTilemapPosCorner(*block, &ix, &iy);
    Vector2 newCornerPos = GetTilePos(ix + dirX, iy + dirY);
    Vector2 newCenterPos = Vector2Add(newCornerPos, Vector2Scale(block->block->center, TILE_W));
    block->targetPos = newCenterPos;
    printf("Move to (%f %f)\n", newCenterPos.x, newCenterPos.y);
    block->oldPos = block->pos;
    block->progress = 0;
}

Rectangle intersect_tiles(ActiveTetramino block, bool* empty) {
    Rectangle result = {0};
    Rectangle tileMapRect = {
        .x = tileMapPosX,
        .y = tileMapPosY,
        .width = tileMapWidth,
        .height = tileMapHeight
    };

    Rectangle blockRect = {
        .x = block.pos.x - block.block->center.x * TILE_W,
        .y = block.pos.y - block.block->center.y * TILE_H,
        .width = TILE_W * BLOCK_SIZE,
        .height = TILE_H * BLOCK_SIZE
    };

    if (CheckCollisionRecs(tileMapRect, blockRect)) {
        bool local_empty = true;
        for (size_t i = 0; i < TILES_X; i++)
        {
            for (size_t j = 0; j < TILES_Y; j++)
            {
                if (IsBlank(tilemap[i][j])) {
                    continue;
                }

                local_empty = false;
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

                            targetRect.x = blockRect.x + x * TILE_W;
                            targetRect.y = blockRect.y + y * TILE_H;

                            if (CheckCollisionRecs(tileRect, targetRect)) {
                                printf("Collision!\n");
                                DrawRectangleRec(targetRect, block.block->color);
                                DrawRectangleRec(tileRect, tilemap[i][j]);
                                return GetCollisionRec(tileRect, targetRect);
                            }
                        }
                    }
                }
            }
        }

        *empty = local_empty;
    }

    return result;
}

void draw_tetramino(ActiveTetramino tetramino) {
    if (tetramino.block == NULL) {
        return;
    }

    int rot_index = tetramino.rot_index;
    int (*block)[4] = tetramino.block->data[rot_index];

    float startX = tetramino.pos.x - tetramino.block->center.x * TILE_W;
    float startY = tetramino.pos.y - tetramino.block->center.y * TILE_H;

    Color bg;
    memcpy(&bg, &(tetramino.block->color), sizeof(Color));
    bg.a = 64;

    for (size_t i = 0; i < BLOCK_SIZE; i++)
    {
        for (size_t j = 0; j < BLOCK_SIZE; j++)
        {
            if (block[i][j] == 0) {
                continue;
            }

            Rectangle coords = {
                .x = startX + j * TILE_W,
                .y = startY + i * TILE_H, 
                .width = TILE_W,
                .height = TILE_H
            };

            DrawRectangleRec(coords, bg);
            DrawRectangleLinesEx(coords, 1, tetramino.block->color);
        }
    }
}

Vector2 StateToCoords(PlanetState state, float scale, Vector2 center) {
    Vector2 result;
    float dist_to_scale = state.distance.value / dist_scale;
    result.x = center.x + dist_to_scale * cos(state.angle.value);
    result.y = center.y + dist_to_scale * sin(state.angle.value);
    return result;
}

void UpdatePlanetState(PlanetState* state, float dt) {
    float dist_acc = distance_acceleration(*state);
    state->distance.speed = new_value(state->distance.speed, dt, dist_acc);
    state->distance.value = new_value(state->distance.value, dt, state->distance.speed);

    float angle_acc = angle_acceleratioin(*state);
    state->angle.speed = new_value(state->angle.speed, dt, angle_acc);
    state->angle.value = new_value(state->angle.value, dt, state->angle.speed);
}

float draw_trajectory() {
    const size_t trajectory_size = 50;
    const int dt = 10;
    PlanetState trajectory[trajectory_size];
    memcpy(&trajectory[0], &planet_state, sizeof(PlanetState));
    Vector2 startPos = StateToCoords(trajectory[0], dist_scale, star_pos);
    Vector2 firstPoint = startPos;
    float maxDist = 0.0f;
    Color color = RED;
    color.a = 127;
    for (size_t i = 1; i < trajectory_size; i++)
    {
        float dist = Vector2Distance(star_pos, startPos);
        if (dist > maxDist)
        {
            maxDist = dist;
        }
        
        memcpy(&trajectory[i], &trajectory[i-1], sizeof(PlanetState));
        for (size_t j = 0; j < dt; j++)
        {
            float local_delta_time = UpdateDeltaTime(trajectory[i]);
            UpdatePlanetState(&trajectory[i], local_delta_time);
        }

        Vector2 endPos = StateToCoords(trajectory[i], dist_scale, star_pos);

        color.a -= 127 / trajectory_size;
        DrawLineEx(startPos, endPos, 2, color);

        if (i > 10 && Vector2Distance(endPos, firstPoint) < TILE_W * 2) {
            return maxDist;
        }

        if (Vector2Distance(endPos, star_pos) < TILE_W * 2)
        {
            return maxDist;
        }
        
        startPos = endPos;
    }

    return maxDist;
}

void DetectPieceTooFar(ActiveTetramino piece) {
    if (Vector2Distance(piece.pos, star_pos) > maxPieceDistance) {
        ResetPlanetState();
        newPieceTextCurrentFrame = 0;
        // GenerateNextTetramino();
    }
}

void InitExplode() {
    float speedMin = 2, speedMax = 6;
    float distMin = 0, distMax = Vector2Distance(GetTilePos(0, 0), star_pos);
    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            if (IsBlank(tilemap[i][j]))
            {
                continue;
            }

            explodeMap[i][j].color = tilemap[i][j];
            explodeMap[i][j].color.a = tilemapAlpha;
            explodeMap[i][j].pos = GetTilePos(i, j);
            float dist = Vector2Distance(explodeMap[i][j].pos, star_pos);
            float angle = Vector2Angle(star_pos, explodeMap[i][j].pos) * PI / 180;
            float speed = Remap(dist, distMin, distMax, speedMin, speedMax);
            explodeMap[i][j].speed.x = speed * cos(angle);
            explodeMap[i][j].speed.y = speed * sin(angle);
        }
    }
}

void UpdateExplode() {
    float acc = 0.01;
    float fadeSpeed = 1;

    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            if (IsBlank(explodeMap[i][j].color)) {
                continue;
            }

            explodeMap[i][j].pos = Vector2Add(explodeMap[i][j].pos, explodeMap[i][j].speed);
            explodeMap[i][j].speed = Vector2Add(explodeMap[i][j].speed, 
                Vector2Scale(Vector2Normalize(explodeMap[i][j].speed), -acc));
            if (explodeMap[i][j].color.a > 0) {
                explodeMap[i][j].color.a -= fadeSpeed;
            }
        }
    }
}

void DrawExplode() {
    for (size_t i = 0; i < TILES_X; i++)
    {
        for (size_t j = 0; j < TILES_Y; j++)
        {
            MovingPoint pt = explodeMap[i][j];
            if (IsBlank(pt.color))
            {
                continue;
            }

            int borderAlpha = Remap(pt.color.a, 0, tilemapAlpha, 0, 255);
            Color borderColor = pt.color;
            borderColor.a = borderAlpha;
            DrawRectangle(pt.pos.x, pt.pos.y, TILE_W, TILE_H, pt.color);
            DrawRectangleLines(pt.pos.x, pt.pos.y, TILE_W, TILE_H, borderColor);
        }   
    }
}

void game_init() {
    gameOver = false;
    explode = false;
    gamePoints = 0;

    gameOverProgress = 0;
    gameOverProgress = 0;
    gameOverColor = RAYWHITE;
    gameOverColor.a = 0;

    newPieceTextCurrentFrame = newPieceTextFrames;

    star_mass = 1.98855 * pow(10, 30);
    gravity_const = 6.67408 * pow(10, -11);
    delta_time = 3600 * 24;
    dist_scale = 6.5 * pow(10, 8);

    maxDeltaTime = 3600 * 24;
    minDeltaTime = 3600;
    maxDist = 1.496 * pow(10, 11);
    minDist = 3.5 * pow(10, 8);

    sliding_tetramino.block = NULL;
    next_tetramino.pos.x = 110;
    next_tetramino.pos.y = 80;

    text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);

    GenerateNextTetramino();
    ResetPlanetState();

    star_pos.x = SCREEN_WIDTH / 2;
    star_pos.y = SCREEN_HEIGHT / 2;

    // tiles
    memset(tilemap, 0, sizeof(tilemap));
    tilemap[TILES_X/2-1][TILES_Y/2-1] = RED;
    tilemap[TILES_X/2-1][TILES_Y/2] = GREEN;
    tilemap[TILES_X/2][TILES_Y/2-1] = YELLOW;
    tilemap[TILES_X/2][TILES_Y/2] = BLUE;

    memset(&tileDeleteInfo, 0, sizeof(TileDeleteInfo));
    deleteProgress = 0;

    memset(&explodeMap, 0, sizeof(explodeMap));

    // camera
    camera.zoom = 1.0f;
    camera.offset = star_pos;
    camera.target = star_pos;
    newZoom = 1.0f;

    printf("%s called\n", __FUNCTION__);
}

screen_t game_update() {
    UpdatePlanetState(&planet_state, delta_time);

    if (IsKeyDown(KEY_UP)) {
        planet_state.angle.speed += pow(10, -9);
    }
    if (IsKeyDown(KEY_DOWN)) {
        planet_state.angle.speed -= pow(10, -9);
    }

    if (IsKeyPressed(KEY_LEFT)) {
        active_tetramino.rot_index = (active_tetramino.rot_index + 3) % 4;  // same as -1 % 4
    } else if (IsKeyPressed(KEY_RIGHT)) {
        active_tetramino.rot_index = (active_tetramino.rot_index + 1) % 4;
    }

    // printf("%f %f %f %.15f %f\n", planet_state.distance.value, planet_state.distance.speed, planet_state.angle.value, planet_state.angle.speed, delta_time);

    Vector2 tetraminoPos = StateToCoords(planet_state, dist_scale, star_pos);
    active_tetramino.pos.x = tetraminoPos.x;
    active_tetramino.pos.y = tetraminoPos.y;

    delta_time = UpdateDeltaTime(planet_state);

    bool empty = false;
    Rectangle res = intersect_tiles(active_tetramino, &empty);

    if (res.width > 0 && res.height > 0) {
        // PlaceTetramino();
        sliding_tetramino.block = active_tetramino.block;
        sliding_tetramino.pos.x = active_tetramino.pos.x;
        sliding_tetramino.pos.y = active_tetramino.pos.y;
        sliding_tetramino.rot_index = active_tetramino.rot_index;
        sliding_tetramino.progress = 1.1;  // to check where to slide it first
        ResetPlanetState();
        GenerateNextTetramino();
    } else if (Vector2Distance(active_tetramino.pos, star_pos) < TILE_W / 2)
    {
        printf("Found new center!\n");
        PlaceTetramino(&active_tetramino);
        ResetPlanetState();
        GenerateNextTetramino();
    }

    MoveSlidingTetramino(&sliding_tetramino);

    DetectPieceTooFar(active_tetramino);

    if (abs(gameOverProgress - gameOverValue) < gameOverAlphaThreshold) {
        printf("Game Over! %d\n", gameOverProgress);
        explode = true;
        InitExplode();
        gameOverProgress = 1000; // dirty hack to init explode once
    }

    if (explode) {
        UpdateExplode();
    }

    if (gameOver) {
        return game_over_screen;
    } else {
        return game_screen;
    }
}

void game_draw() {
    BeginDrawing();

    // zoom interpolation
    float dif = camera.zoom - newZoom;
    if (fabs(dif) > zoomSpeed)
    {
        int dir = dif < 0 ? 1 : -1;
        camera.zoom += zoomSpeed * dir;
    }

    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

    draw_tetramino(active_tetramino);
    draw_tetramino(sliding_tetramino);
    float maxDist = draw_trajectory();
    float targetScale = (SCREEN_HEIGHT / 2 - frameOffset) / maxDist;

    if (targetScale <= 1.1f && targetScale > 0.5f) {
        newZoom = targetScale;
    }

    if (explode) {
        DrawExplode();
    } else {
        draw_tilemap();
    }

    EndMode2D();

    DrawText("Next:", 20, 60, 20, GRAY);
    draw_tetramino(next_tetramino);

    if (explode) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gameOverColor);
        gameOverColor.a += 2;

        if (gameOverColor.a >= 250) {
            gameOver = true;
        }

        char* gameOverText = "GAME OVER!";
        Vector2 text_size = MeasureTextEx(GetFontDefault(), gameOverText, 20, 1);
        DrawText(gameOverText, SCREEN_WIDTH / 2 - text_size.x / 2, text_size.y + 10, 20, BLACK);
    }

    // draw UI after camera
    if (newPieceTextCurrentFrame < newPieceTextFrames) {
        char* newPieceText = "Here's your piece\nDon't lose it again!";
        Vector2 size = MeasureTextEx(GetFontDefault(), newPieceText, 20, 1);
        DrawText(newPieceText, SCREEN_WIDTH / 2 - size.x / 2, SCREEN_HEIGHT / 2 - size.y / 2, 20, GRAY);
        newPieceTextCurrentFrame += 1;
    }

    char points_text[64] = {0};
    snprintf(points_text, 63, "Score: %d", gamePoints);
    DrawText(points_text, 20, 20, 20, GRAY);

    EndDrawing();
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
