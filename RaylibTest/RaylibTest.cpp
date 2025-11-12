#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <raylib.h>
#include <vector>
#include <deque>
#include <set>
#include <String>
#include "raymath.h"


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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
    float currentMultiplier = 1.0f;

    Item(string n, double cps, double cost) {
        name = n;
        cpsPerItem = cps;
        baseCost = cost;
        currentCost = cost;
    }

    double GetContribuition() {
        return (double)count * cpsPerItem * currentMultiplier;
    }

    void Buy(double& playerCur) {
        if (playerCur >= currentCost) {
            playerCur -= currentCost;
            count++;
            currentCost *= costMultiplier;

        }
    }
};

enum upgradeType {
    ClickUpgrade,
    ItemUpgrade
};

struct Upgrade {
    string name;
    double cost;

    upgradeType type;
    float multiplier;
    int targetItemIndex = -1;

    Upgrade(string n, double c, upgradeType t, float m, int target = -1)
        : name(n), cost(c), type(t), multiplier(m), targetItemIndex(target) {};
};

struct FloatingText {
    Vector2 position;
    float lifetime;
    string text;

    FloatingText(Vector2 pos, float life, string txt)
        : position(pos), lifetime(life), text(txt) {}
};

typedef enum TextAlignment {
    ALIGN_LEFT      = 0,
    ALIGN_TOP       = 0,
    ALIGN_CENTER    = 1,
    ALIGN_MIDDLE    = 1,
    ALIGN_RIGHT     = 2,
    ALIGN_BOTTOM    = 2
} TextAlignment;

typedef enum GameScreen { LOGO = 0, MENU, GAMEPLAY, SHOP } GameScreen;

Color darkPurple = Color{ 34, 24, 40, 16 };

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Create
    InitWindow(screenWidth, screenHeight, "Caio Clicker - V.0.0.0.1");
    SetTargetFPS(60);

    // Definindo a tela inicial do jogo
    // GameScreen currentScreen = LOGO; << Pular a splash screen por enquanto
    GameScreen currentScreen = GAMEPLAY;

    // Moeda
    double caios = 99999999999;
    double totalCPS = 0.0;
    double clickPower = 1.0;

    // Texto do clique
    vector<FloatingText> floatingTexts;
    const float ftextLifetime = 0.5f;

    // Upgrades
    vector<Upgrade> upgrades;
    upgrades.push_back(Upgrade("Trufa do sebastião", 100, ClickUpgrade, 2.0f));
    upgrades.push_back(Upgrade("Brownie do Kleber", 500, ClickUpgrade, 2.5f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));
    upgrades.push_back(Upgrade("Almoço do CDR", 1000, ClickUpgrade, 10.0f));

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
    Rectangle back = { 10, 10, 50, 50 };

    // Timers

    // UI
    float uiX = 450.f, uiY = 10.f, buttonWidth = 312.f, buttonHeight = 50.f, buttonSpacing = 2.f;

    // Textos
    Font font = LoadFontEx("m5x7.ttf", 72, 0, 250);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    int fontSpacing = 1;
    GuiSetFont(font);
    
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
        case GAMEPLAY:
        {
            if (CheckCollisionPointCircle(MousePos, button.position, button.radius)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    button.radius += 10;
                    caios += clickPower;

                    string textToShow = TextFormat("+%.0f", clickPower);
                    floatingTexts.push_back(FloatingText(MousePos, ftextLifetime, textToShow));
                }
            }

            for (int i = 0; i < floatingTexts.size(); i++)
            {
                // Pega o texto atual
                FloatingText& txt = floatingTexts[i];

                // Atualiza o timer e a posição
                txt.lifetime -= GetFrameTime();
                txt.position.y -= 20 * GetFrameTime(); // Flutua para cima

                // Se o tempo de vida acabou, apaga ele da lista
                if (txt.lifetime <= 0)
                {
                    floatingTexts.erase(floatingTexts.begin() + i);
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
            // Padrões do texto
            GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
            GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
            GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
            GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(DARKGRAY));

            switch (currentScreen) {
                case LOGO:
                {
                    frameCounter++;
                    if (frameCounter > 120) {
                        currentScreen = MENU;
                    }
                    ClearBackground(BLACK);
                    // SplashScreen
                    {
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
                        GuiLabel({ 0,0,screenWidth, screenHeight }, "GAME BY SAMTLOKOMEMO");
                    }

                } break;
                case MENU:
                {
                    ClearBackground(DARKGREEN);

                    // Título
                    {
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
                        GuiLabel({ 0, 0, (float)screenWidth, 100 }, "CAIO CLICKER");
                    }

                    // Botão jogar
                    {
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
                        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(RED));
                        float tamanhoDoTexto = MeasureText("JOGAR", 24);
                        if (GuiButton({ screenWidth / 2 - 60, screenHeight / 4, tamanhoDoTexto + 30, 36 }, "JOGAR")) currentScreen = GAMEPLAY;
                    }
                } break;
                case GAMEPLAY:
                {
                    ClearBackground(darkPurple);

                    // Título 
                    {
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
                        GuiLabel({ 0, 0, (float)screenWidth, 100 }, "CAIO CLICKER");
                    }
                    // Botão da loja
                    {
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
                        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(LIME));
                        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(GREEN));
                        GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(DARKGREEN));
                        if (GuiButton({ screenWidth - 75, screenHeight - 75, 50.f, 50.f }, "$")) currentScreen = SHOP;
                    }

                    // Botão do caio
                    DrawCircle(button.position.x, button.position.y, button.radius, button.color);

                    // Texto do clique
                    for (const auto& txt : floatingTexts) // 'const auto&' é um jeito rápido e eficiente
                    {
                        // Calcula o fade para ESTE texto
                        float fadeAlpha = txt.lifetime / ftextLifetime;
                        Color fadeColor = Fade(WHITE, fadeAlpha);

                        // Desenha ESTE texto
                        DrawText(txt.text.c_str(), txt.position.x, txt.position.y, 20, fadeColor);
                    }
                } break;
                case SHOP:
                {
                    ClearBackground(darkPurple);

                    // --- 1. BOTÃO DE VOLTAR E TÍTULO (Seu código está ótimo) ---
                    {
                        // Botão de voltar
                        {
                            GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
                            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                            GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(DARKGRAY));
                            GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(DARKPURPLE));
                            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(LIGHTGRAY));
                            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(GRAY));
                            GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(DARKGRAY));
                            if (GuiButton({ 10, 10, 30, 30 }, "<")) currentScreen = GAMEPLAY;
                        }
                        // Título da loja
                        {
                            GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
                            GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
                            GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
                            GuiLabel({ 100, 20, (float)screenWidth, (float)screenHeight }, "LOJA\n\n\n DO\n\n\nCAIO");
                        }
                    }


                    // --- 2. ÁREA DOS UPGRADES (OS QUADRADOS) ---
                    // Vamos desenhar os upgrades PRIMEIRO, no topo da coluna da loja.
                    {
                        // Define o estilo dos botões de upgrade
                        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(SKYBLUE));
                        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(BLUE));
                        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(DARKBLUE));
                        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Tamanho do texto para os ícones

                        // Define o tamanho dos botões quadrados
                        float upgradeSize = 48.0f;
                        float upgradeSpacing = 4.0f;
                        float buttonY = 0.f, buttonX = 0.f;

                        for (int i = 0; i < upgrades.size(); i++)
                        {
                            Upgrade& upg = upgrades[i];

                            // SÓ MOSTRE O UPGRADE SE ELE NÃO FOI COMPRADO
                            // Calcula a posição X (horizontal)
                            buttonX = uiX + (i % 6) * (upgradeSize + upgradeSpacing);
                            // A posição Y é fixa (no topo da coluna)
                            buttonY = uiY + (i / 6) * (upgradeSize + upgradeSpacing);
                                

                            Rectangle buttonRect = { buttonX, buttonY, upgradeSize, upgradeSize };

                            if (caios < upg.cost) GuiDisable();

                            // --- Problema de Texto vs. Ícone ---
                            // Texto (nome/custo) não cabe em um quadrado de 48x48.
                            // O Cookie Clicker usa ÍCONES.
                            // Solução: Usamos a primeira letra e mostramos o resto com um "Tooltip".
                            const char* iconText = TextFormat("%c", upg.name[0]);
                            const char* toolTipText = TextFormat("%s | Custo: %.0f", upg.name.c_str(), upg.cost);

                            // Mostra o "tooltip" se o mouse estiver em cima
                            if (CheckCollisionPointRec(GetMousePosition(), buttonRect))
                            {
                                GuiTooltip(buttonRect);
                            }

                            // Desenha o botão e checa o clique
                            if (GuiButton(buttonRect, iconText))
                            {
                                caios -= upg.cost;
                                if (upg.type == ClickUpgrade)
                                {
                                    clickPower *= upg.multiplier;
                                }
                                else if (upg.type == ItemUpgrade)
                                {
                                    int targetIndex = upg.targetItemIndex;
                                    // Checa se o índice é válido
                                    if (targetIndex >= 0 && targetIndex < itens.size())
                                    {
                                        itens[targetIndex].currentMultiplier *= upg.multiplier;
                                    }
                                }
                                upgrades.erase(upgrades.begin() + i);
                            }
                            GuiEnable();
                        }

                    // --- 3. ÁREA DOS ITENS (OS RETÂNGULOS) ---
                    // Agora, desenhamos a lista de itens ABAIXO da barra de upgrades.
                        // Define a posição Y inicial para os itens
                        // (Abaixo dos upgrades + 10 pixels de espaço)
                        float itemUiY = uiY + buttonY + 42.0f;

                        // Define o estilo dos botões de item
                        GuiSetStyle(DEFAULT, TEXT_SIZE, 12);
                        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
                        // (Seu outro código GuiSetStyle para os botões de item...)
                        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK)); // Mudei para preto para ler melhor

                        for (int i = 0; i < itens.size(); i++)
                        {
                            Item& item = itens[i];
                            // --- CORREÇÃO DE BUG ---
                            // Seu cálculo `(buttonHeight * buttonSpacing)` estava errado.
                            // O correto é `i * (altura + espaço)`.
                            float itemY = itemUiY + (i * (buttonHeight + buttonSpacing));
                            Rectangle buttonRect = { uiX, itemY, buttonWidth, buttonHeight };
                            if (caios < item.currentCost) GuiDisable();
                            const char* buttonText = TextFormat("%s (%d) | Custo: %.0f",
                                item.name.c_str(),
                                item.count,
                                item.currentCost);
                            if (GuiButton(buttonRect, buttonText)) item.Buy(caios);
                            GuiEnable(); // Mova o GuiEnable para dentro do loop
                        }
                    }
                    // Reseta o estilo padrão no final para não afetar outras telas
                    GuiSetStyle(DEFAULT, TEXT_SIZE, 10); // 10 é o padrão do raygui

                } break;
            }
            // Textos dos valores
            {
                GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
                GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
                GuiLabel({ 0, screenHeight - 100, (float)screenWidth, 20 }, TextFormat("você tem %.0f caios", caios));
                GuiLabel({ 0, screenHeight - 100, (float)screenWidth, 45 }, TextFormat("você ganha %.1f  caios por segundo", totalCPS));
            }
        EndDrawing();
        }
    UnloadFont(font);
    CloseWindow();
    return 0;
}



