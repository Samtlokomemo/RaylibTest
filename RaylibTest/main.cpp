#include "Game.h"
using namespace Config;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Caio Clicker - V.0.0.0.1");
    SetTargetFPS(60);

    Game game;
    game.Init();

    while (!WindowShouldClose()) {

        game.Update(); 
        game.Draw();   

    }
    CloseWindow();
    return 0;
}