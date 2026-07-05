#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "electron.h"
#include "barrier.h"

#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

using namespace std;

float TransmissionProbability(float E, float V0, float m, float hbar, float L) {
    float diff = V0 - E;
    if (fabsf(diff) < 1e-6f) diff = 1e-6f;

    if (diff > 0.0f) {
        float kappa = sqrtf(2.0f * m * diff) / hbar;
        float s = sinhf(kappa * L);
        return 1.0f / (1.0f + (V0 * V0 * s * s) / (4.0f * E * diff));
    } else {
        float diff2 = -diff;
        float k2 = sqrtf(2.0f * m * diff2) / hbar;
        float s = sinf(k2 * L);
        return 1.0f / (1.0f + (V0 * V0 * s * s) / (4.0f * E * diff2));
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    float HBAR = 1000.0f;
    float spawnVelocity = 300.0f;

    Electron electron;
    electron.position = {0, 300};
    electron.velocity = {spawnVelocity, 0};
    electron.mass = 1.0f;

    Barrier barrier;
    barrier.position = {400, 0};
    barrier.width = 20;
    barrier.height = 46000.0f;

    bool guiOpened = false;
    float lastChance = -1.0f;

    InitWindow(800, 600, "Quantum Tunneling");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        float E = 0.5f * electron.mass * electron.velocity.x * electron.velocity.x;

        if (!electron.checked && electron.position.x >= barrier.position.x) {
            electron.checked = true;
            float chance = TransmissionProbability(E, barrier.height, electron.mass, HBAR, barrier.width);
            lastChance = chance;
            float random = static_cast<float>(rand()) / RAND_MAX;
            cout << "T (chance) = " << chance << "   roll = " << random << '\n';
            if (random < chance) {
                electron.passed = true;
            } else {
                electron.velocity.x *= -1;
            }
        }

        electron.position.x += electron.velocity.x * dt;
        electron.position.y += electron.velocity.y * dt;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangleV(barrier.position, {barrier.width, 800}, Fade(WHITE, 0.3f + 0.5f * (barrier.height / 60000.0f)));
        DrawCircleV(electron.position, 15, RED);

        DrawText(TextFormat("E (Energy) = %.0f", E), 10, 10, 20, GRAY);
        if (lastChance >= 0.0f) {
            DrawText(TextFormat("T (Chance) = %.2f%%", lastChance * 100.0f), 10, 35, 20, GRAY);
        }

        if (GuiButton((Rectangle){770, 0, 30, 30}, "M")) {
            guiOpened = !guiOpened;
        }

        if (guiOpened) {
            GuiPanel((Rectangle){550, 0, 300, 600}, "PARAMETERS");

            float x = 565;
            float w = 220;
            float y = 40;
            float step = 40;

            GuiSlider((Rectangle){x, y, w, 20}, "", TextFormat("%.2f", electron.mass), &electron.mass, 0.2f, 5.0f);
	    GuiLabel({x, y+20, w, 20}, "mass");
            y += step;

            GuiSlider((Rectangle){x, y, w, 20}, "", TextFormat("%.0f", spawnVelocity), &spawnVelocity, 50.0f, 600.0f);
            GuiLabel({x, y+20, w, 20}, "velocity");
	    y += step;

            GuiSlider((Rectangle){x, y, w, 20}, "", TextFormat("%.0f", barrier.height), &barrier.height, 40000.0f, 70000.0f);
            GuiLabel({x, y+20, w, 20}, "barrier height");
	    y += step;

            GuiSlider((Rectangle){x, y, w, 20}, "", TextFormat("%.1f", barrier.width), &barrier.width, 2.0f, 80.0f);
            GuiLabel({x, y+20, w, 20}, "barrier width");
	    y += step;

            GuiSlider((Rectangle){x, y, w, 20}, "", TextFormat("%.0f", HBAR), &HBAR, 200.0f, 3000.0f);
	    GuiLabel({x, y+20, w, 20}, "hbar");
            y += step + 10;

            if (GuiButton((Rectangle){x, y, w, 30}, "spawn electron")) {
                electron.position = {0, 300};
                electron.velocity = {spawnVelocity, 0};
                electron.checked = false;
                electron.passed = false;
                lastChance = -1.0f;
            }
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
