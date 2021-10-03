#ifndef TETRAMINO_H
#define TETRAMINO_H

#include "raylib.h"

#define BLOCK_SIZE 4
#define TILE_W 16 // px
#define TILE_H 16

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

extern Tetramino Z_Block;
extern Tetramino O_Block;
extern Tetramino I_Block;
extern Tetramino L_Block;
extern Tetramino J_Block;
extern Tetramino S_Block;
extern Tetramino T_Block;

extern Tetramino* Blocks[];

Tetramino* GetRandomBlock();

void draw_tetramino(ActiveTetramino tetramino);

#endif /* TETRAMINO_H */
