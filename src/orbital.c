#include "orbital.h"

#include "math.h"


float distance_acceleration(PlanetState state, float star_mass) {
    float gravity_const = 6.67408 * pow(10, -11);
    return state.distance.value * pow(state.angle.speed, 2) -
    (gravity_const * star_mass) / pow(state.distance.value, 2);
}

float angle_acceleratioin(PlanetState state) {
    return -2.0f * state.distance.speed * state.angle.speed / state.distance.value;
}

float new_value(float current_value, float delta_time, float derivative) {
    return current_value + delta_time * derivative;
}

Vector2 StateToCoords(PlanetState state, float scale, Vector2 center) {
    Vector2 result;
    float dist_to_scale = state.distance.value / scale;
    result.x = center.x + dist_to_scale * cos(state.angle.value);
    result.y = center.y + dist_to_scale * sin(state.angle.value);
    return result;
}

void UpdatePlanetState(PlanetState* state, float dt, float star_mass) {
    float dist_acc = distance_acceleration(*state, star_mass);
    state->distance.speed = new_value(state->distance.speed, dt, dist_acc);
    state->distance.value = new_value(state->distance.value, dt, state->distance.speed);

    float angle_acc = angle_acceleratioin(*state);
    state->angle.speed = new_value(state->angle.speed, dt, angle_acc);
    state->angle.value = new_value(state->angle.value, dt, state->angle.speed);
}
