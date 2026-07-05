#pragma once

#include "raylib.h"

class Electron {
    public:
    Vector2 position;

    Vector2 velocity;

    float mass;

    bool passed = false;

    bool checked = false;
};
