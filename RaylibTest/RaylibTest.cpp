#include <iostream>
#include <raylib.h>
#include <vector>
#include <String>
#include "raymath.h"

using namespace std;

struct CircleButton {
    Vector2 position;
    float radius, maxRadius;
    Color color;
};

struct Timer {
    float currentTime = 0.0f, interval;
    
    Timer(float targetInterval) {
        interval = targetInterval;
    }

    bool Tick() {
        currentTime += GetFrameTime();
        if (currentTime >= interval) {
            currentTime -= interval;
            return true;
        }
        return false;
    }

    void Reset() {
        currentTime = 0.0f;
    }
};

struct SquareButton {
    Vector2 position, size;
    Color color;
};

struct Item {
    string name;
    int count = 0;
    double cpsPerItem = 0.1;
    double baseCost = 10.0;
    double currentCost = 10.0;
    double costMultiplier = 1.15;

    Item(string n, double cps, double cost) {
        name = n;
        cpsPerItem = cps;
        baseCost = cost;
        currentCost = cost;
    }

    double GetContribuition() {
        return (double)count * cpsPerItem;
    }

    void Buy(double& playerCur) {
        if (playerCur >= currentCost) {
            playerCur -= currentCost;
            count++;
            currentCost *= costMultiplier;

        }
    }
};

typedef enum TextAlignment {
    ALIGN_LEFT      = 0,
    ALIGN_TOP       = 0,
    ALIGN_CENTER    = 1,
    ALIGN_MIDDLE    = 1,
    ALIGN_RIGHT     = 2,
    ALIGN_BOTTOM    = 2
} TextAlignment;

void DrawTextInRect(Font font, const char* text, Rectangle container, int fontSize, int fontSpacing, Color color, TextAlignment hAlign, TextAlignment vAlign)
{
    // 1. Medir o tamanho do texto
    Vector2 textSize = MeasureTextEx(font, text, (float)fontSize, (float)fontSpacing);

    // 2. Calcular a Posição X (Horizontal)
    // A lógica 'Lerp' do exemplo é a mais elegante para isso.
    // (hAlign * 0.5f) converte 0, 1, 2 para 0.0, 0.5, 1.0
    float posX = container.x + Lerp(0.0f, container.width - textSize.x, (float)hAlign * 0.5f);

    // 3. Calcular a Posição Y (Vertical)
    float posY = container.y + Lerp(0.0f, container.height - textSize.y, (float)vAlign * 0.5f);

    // 4. Desenhar o texto na posição calculada
    DrawTextEx(font, text, { posX, posY }, (float)fontSize, (float)fontSpacing, color);
}

typedef enum GameScreen { LOGO = 0, MENU, GAMEPLAY, SHOP } GameScreen;

Color darkPurple = Color{ 34, 24, 40, 16 };

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Create
    InitWindow(screenWidth, screenHeight, "Caio Clicker - V.0.0.0.1");
    SetTargetFPS(60);

    // Definindo a tela inicial do jogo
    GameScreen currentScreen = LOGO;

    // Moeda
    double caios = 0;
    double totalCPS = 0.0;

    // Itens
    vector<Item> itens;
    itens.push_back(Item("Professora de Química", 0.1, 10));
    itens.push_back(Item("Beijo do bubulane", 0.3, 30));
    itens.push_back(Item("Cama de casal", 0.5, 50));
    itens.push_back(Item("Pão", 1, 100));

    // Botao do caio
    CircleButton button;
    button.position = { screenWidth / 2 , screenHeight / 2 };
    button.maxRadius = 60.f;
    button.radius = 50.f;
    button.color = RED;

    // Botao da loja
    Rectangle loja = { screenWidth - 75, screenHeight - 75, 50.f, 50.f };
    Rectangle back = { 10, 10, 50, 50 };

    // Timers

    // UI
    float uiX = 450.f, uiY = 10.f, buttonWidth = 300.f, buttonHeight = 50.f, buttonSpacing = 2.f;

    // Textos
    Font font = LoadFontEx("m5x7.ttf", 72, 0, 250);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    int fontSpacing = 1;
    
    // Checagem da fonte
    {
        if (font.texture.id == 0)
        {
            TraceLog(LOG_WARNING, "Fonte não encontrada. Usando fonte padrão.");
        }
        else
        {
            TraceLog(LOG_INFO, "Fonte carregada com sucesso!");
        }
    }

    // Alignment
    const char* textAlignNameH[] = { "Left", "Center", "Right" };
    const char* textAlignNameV[] = { "Top", "Middle", "Bottom" };

    TextAlignment hAlign = ALIGN_CENTER;
    TextAlignment vAlign = ALIGN_MIDDLE;

    // Variáveis gerais
    Vector2 MousePos = { 0 };
    int frameCounter = 0;
    
    // GameLoop
    while (WindowShouldClose() == false) {
        MousePos = GetMousePosition();
        
        // Cálculo dos caios
        totalCPS = 0;
        for (int i = 0; i < itens.size(); i++)
        {   
            totalCPS += itens[i].GetContribuition();
        }
        caios += totalCPS * GetFrameTime();

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
        }

        // Render
        BeginDrawing();
            switch (currentScreen) {
            case LOGO:
            {
                ClearBackground(RAYWHITE); 
                Rectangle screenRect = { 0, 0, (float)screenWidth, (float)screenHeight };
                DrawTextInRect(font, "GAME BY SAMTLOKOMEMO", screenRect, 32, fontSpacing, BLACK, ALIGN_CENTER, ALIGN_MIDDLE);
            } break;
            case MENU:
            {
                ClearBackground(DARKGREEN);
                // Container para o título: começa em y=50, tem 40px de altura
                Rectangle titleBox = { 0, 50, (float)screenWidth, 40 };
                DrawTextInRect(font, "CAIO CLICKER", titleBox, 40, fontSpacing, WHITE, ALIGN_CENTER, ALIGN_TOP);

                // Container para o subtítulo: começa em y=300, tem 24px de altura
                Rectangle subtitleBox = { 0, 300, (float)screenWidth, 24 };
                DrawTextInRect(font, "aperte ENTER para jogar", subtitleBox, 24, fontSpacing, WHITE, ALIGN_CENTER, ALIGN_TOP);
            } break;
            case GAMEPLAY:
            {
                ClearBackground(darkPurple);
                DrawCircle(button.position.x, button.position.y, button.radius, button.color);
                DrawRectangleRec(loja, DARKGREEN);
                DrawTextEx(font, "CAIO CLICKER", { screenWidth / 2 - 170 }, 50, fontSpacing, WHITE);
                DrawTextEx(font, TextFormat("VOCE TEM %.0f caios", caios), { screenWidth / 2 - 100, screenHeight - 100 }, 20, fontSpacing, WHITE);
                DrawTextEx(font, TextFormat("VOCE ganha %.1f  caios por segundo", totalCPS), { screenWidth / 2 - 100, screenHeight - 60 }, 20, fontSpacing, WHITE);
            } break;
            case SHOP:
            {
                ClearBackground(darkPurple);

                // Botão de sair da loja
                DrawRectangleRec(back, DARKGREEN);
                if (CheckCollisionPointRec(GetMousePosition(), back) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentScreen = GAMEPLAY;
                
                DrawTextEx(font, "LOJA\n DO\nCAIO", { screenWidth / 2 - 150, 50 }, 50, fontSpacing, WHITE);
                
                for (int i = 0; i < itens.size(); i++)
                {
                    Item& item = itens[i];
                    // Criando o botão dinamicamente de acordo com o número de itens
                    float buttonY = uiY + (i * (buttonHeight * buttonSpacing));
                    Rectangle buttonRect = {uiX, buttonY, buttonWidth, buttonHeight};

                    // Comprando o item
                    bool mouseOver = CheckCollisionPointRec(GetMousePosition(), buttonRect);
                    if (mouseOver && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        item.Buy(caios);
                    }

                    // Mudando a cor do botão se o item puder ser comprado
                    Color buttonColor = LIME;
                    if (caios < item.currentCost) buttonColor = GRAY;
                    else if (mouseOver) buttonColor = GREEN;
                    DrawRectangleRec(buttonRect, buttonColor);

                    // Texto do botão
                    // Container para o nome do item (metade de cima do botão, com 10px de padding)
                    Rectangle nameBox = { buttonRect.x + 10, buttonRect.y, buttonRect.width - 20, buttonRect.height / 2 };
                    DrawTextInRect(font, TextFormat("%s (%d)", item.name.c_str(), item.count), nameBox, 20, fontSpacing, BLACK, ALIGN_LEFT, ALIGN_MIDDLE);

                    // Container para o custo (metade de baixo do botão, com 10px de padding)
                    Rectangle costBox = { buttonRect.x + 10, buttonRect.y + (buttonRect.height / 2), buttonRect.width - 20, buttonRect.height / 2 };
                    DrawTextInRect(font, TextFormat("Custo: %.0f", item.currentCost), costBox, 15, fontSpacing, DARKGRAY, ALIGN_RIGHT, ALIGN_MIDDLE);
                }
            } break;
            }
        EndDrawing();
    }
    UnloadFont(font);
    CloseWindow();
    return 0;
}