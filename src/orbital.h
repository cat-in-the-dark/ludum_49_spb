#ifndef ORBITAL_H
#define ORBITAL_H

#include "raylib.h"

typedef struct {
    float value;
    float speed;
} SpVector2;

typedef struct {
    SpVector2 distance;
    SpVector2 angle;
} PlanetState;

float distance_acceleration(PlanetState state, float star_mass);

float angle_acceleratioin(PlanetState state);

float new_value(float current_value, float delta_time, float derivative);

Vector2 StateToCoords(PlanetState state, float scale, Vector2 center);

void UpdatePlanetState(PlanetState* state, float dt, float star_mass);

#endif /* ORBITAL_H */
