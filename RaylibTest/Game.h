#pragma once
#include <vector>
#include <string>
#include "raylib.h"
#include "raygui.h" // Se precisar de raygui aqui, mas geralmente é no cpp
#include "Item.h"
#include "Upgrade.h"
#include "FloatingText.h"
#include "ClickerButton.h"
#include "Constants.h"

// Enums podem ficar aqui ou em um arquivo separado "Enums.h" se preferir
typedef enum TextAlignment {
    ALIGN_LEFT = 0,
    ALIGN_TOP = 0,
    ALIGN_CENTER = 1,
    ALIGN_MIDDLE = 1,
    ALIGN_RIGHT = 2,
    ALIGN_BOTTOM = 2
} TextAlignment;

typedef enum GameScreen { LOGO = 0, MENU, GAMEPLAY, SHOP, ACHIEVEMENTS } GameScreen;

class Game {
public:
    // --- Variáveis de Estado do Jogo ---
    double caios;
    double totalCPS;
    double clickPower;

    GameScreen currentScreen;
    int clicks;
    int clickAchieve;

    // --- Conteúdo ---
    std::vector<Upgrade> upgrades;
    std::vector<Item> itens;
    std::vector<FloatingText> floatingTexts;

    // --- Recursos e UI ---
    Font font;
    ClickerButton* botaoDoCaio; // Ponteiro para inicializar depois

    // Variáveis de UI
    float uiX, uiY, buttonWidth, buttonHeight, buttonSpacing;
    Rectangle backButtonRect;

    // Configurações de Texto Flutuante
    const float ftextLifetime = 0.5f;

    // Variável para controle de frames (usada na tela de LOGO)
    int frameCounter;

    // --- Métodos ---
    Game();  // Construtor
    ~Game(); // Destrutor

    void Init();
    void Update();
    void Draw();

    void InitGameContent(); // Função para popular itens/upgrades
    void SaveGame();
    void LoadGame();
    void ResetSave();

    // Métodos auxiliares de desenho (opcional, para organizar o Draw)
    void DrawLogo();
    void DrawMenu();
    void DrawGameplay();
    void DrawShop();
    void DrawAchievements();
    void DrawHUD(); // Textos de valores (caios, cps)
private:
    void UpdateLogo();
    void UpdateMenu();
    void UpdateGameplay();
    void UpdateShop();
    void UpdateAchievments();
    void UpdateCaios();
};