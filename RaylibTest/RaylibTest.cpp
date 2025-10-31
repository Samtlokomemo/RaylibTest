#include <iostream>
#include <raylib.h>

using namespace std;

typedef struct CircleButton {
    Vector2 position;
    float radius, maxRadius;
    Color color;
};

typedef struct SquareButton {
    Vector2 position, size;
    Color color;
};

typedef enum GameScreen { LOGO = 0, MENU, GAMEPLAY, SHOP } GameScreen;

int main() {

    GameScreen currentScreen = LOGO;

    Color darkPurple = Color{34, 24, 40, 16};

    const int screenWidth = 800;
    const int screenHeight = 600;

    // Itens
    int caios = 0;
    int teachers = 0;

    // Botao do caio
    CircleButton button;
    button.position = { screenWidth / 2 , screenHeight / 2 };
    button.maxRadius = 60.f;
    button.radius = 50.f;
    button.color = RED;

    // Botao da loja
    Rectangle loja = { screenWidth - 75, screenHeight - 75, 50.f, 50.f };
    Rectangle buyTeacher = { 50, 50, 100, 50 };
    Rectangle back = { 10, 10, 50, 50 };


    Vector2 MousePos = { 0 };

    // Create
    InitWindow(screenWidth, screenHeight, "caio clicker");

    int frameCounter = 0;
    SetTargetFPS(60);
    
    // GameLoop
    while (WindowShouldClose() == false) {
        MousePos = GetMousePosition();

        // TROCA DAS TELAS
        switch (currentScreen) {
            case LOGO:
            {
                frameCounter++;
                if (frameCounter > 120) {
                    currentScreen = MENU;
                }
            } break;
            case MENU:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                if (CheckCollisionPointCircle(MousePos, button.position, button.radius)) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        button.radius += 10;
                        caios += 1;
                    }
                }
                if (CheckCollisionPointRec(MousePos, loja)) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        // Ir para a cena da loja
                        currentScreen = SHOP;
                    }
                }


                if (button.radius > 50) {
                    button.radius--;
                }
                if (button.radius > button.maxRadius) {
                    button.radius = button.maxRadius;
                }
            } break;
            case SHOP:
            {
                if (CheckCollisionPointRec(MousePos, back)) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        // Ir para a cena da loja
                        currentScreen = GAMEPLAY;
                    }
                }

                if (CheckCollisionPointRec(MousePos, buyTeacher)) {
                    buyTeacher.width += 10;
                    buyTeacher.height += 10;
                    teachers += 1;
                }

                if (buyTeacher.width > 100) {
                    buyTeacher.width--;
                    buyTeacher.height--;
                }

                if (buyTeacher.width > 120) {
                    buyTeacher.width = 120;
                    buyTeacher.height = 60;
                }
            } break;
        }

        // Render
        BeginDrawing();
            switch (currentScreen) {
            case LOGO:
            {
                ClearBackground(RAYWHITE);
                DrawText("GAME BY SAMTLOKOMEMO", screenWidth / 2 - 200, screenHeight / 2, 40, BLACK);
            } break;
            case MENU:
            {
                ClearBackground(DARKGREEN);
                DrawText("CAIO CLICKER", screenWidth / 2 - 170, 50, 50, WHITE);
                DrawText("aperte ENTER para jogar", screenWidth / 2 - 200, 300, 30, WHITE);
            } break;
            case GAMEPLAY:
            {
                ClearBackground(darkPurple);
                DrawCircle(button.position.x, button.position.y, button.radius, button.color);
                DrawRectangleRec(loja, DARKGREEN);
                DrawText("CAIO CLICKER", screenWidth / 2 - 170, 50, 50, WHITE);
                DrawText(TextFormat("VOCE TEM %02i caios", caios), screenWidth / 2 - 100, screenHeight - 100, 20, WHITE);
            } break;
            case SHOP:
            {
                ClearBackground(darkPurple);
                DrawRectangleRec(back, DARKGREEN);
                DrawRectangleRec( buyTeacher, DARKBLUE);
                DrawText("LOJA DO CAIO", screenWidth / 2 - 170, 50, 50, WHITE);
            } break;
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}