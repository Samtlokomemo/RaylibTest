#pragma once
#include "raylib.h"

namespace Config {
    // Janela
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const int TARGET_FPS = 60;

    // Gameplay
    const float AUTO_SAVE_INTERVAL = 60.0f;
    const float TEXT_FLOAT_LIFETIME = 0.5f;

    // Cores
    const Color darkPurple = { 34, 24, 40, 16 };
    const Color COLOR_BTN_LIME = { 0, 228, 48, 255 };
}