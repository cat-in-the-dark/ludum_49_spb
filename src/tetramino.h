#ifndef TETRAMINO_H
#define TETRAMINO_H

#include "raylib.h"

typedef struct {
    Color color;
    Vector2 center; // in tiles;
    int data[4][4][4];
} Tetramino;

extern Tetramino* Blocks[];

Tetramino* GetRandomBlock();

#endif /* TETRAMINO_H */
