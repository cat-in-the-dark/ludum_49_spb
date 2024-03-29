#include <string.h>

#include "tetramino.h"
#include "const.h"

Tetramino I_Block, J_Block, L_Block, O_Block, S_Block, T_Block, Z_Block;

Tetramino* Blocks[] = {&I_Block, &L_Block, &J_Block, &O_Block, &S_Block, &T_Block, &Z_Block};

Tetramino* GetRandomBlock() {
    return Blocks[GetRandomValue(0, ARR_SIZE(Blocks) - 1)];
}

void init_tetraminos() {
    I_Block = (Tetramino) {
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

    J_Block = (Tetramino) {
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

    L_Block = (Tetramino) {
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

    O_Block = (Tetramino) {
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

     S_Block = (Tetramino) {
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

     T_Block = (Tetramino) {
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

    Z_Block = (Tetramino) {
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
    bg.a = TILEMAP_ALPHA;

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
            DrawRectangleLinesEx(coords, 2, tetramino.block->color);
        }
    }
}
