#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"
#include <cstdio>
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

using namespace Config;
// Nota: Se der erro de redefinição do RAYGUI_IMPLEMENTATION, 
// mova o define e o include para um arquivo separado ou mantenha no main se for o único lugar.
// Mas como estamos movendo tudo, o ideal é que o Game.cpp gerencie a GUI se ele for o único a desenhar.
// Se der problema, remova o #define daqui e deixe onde estava, mas garanta que raygui.h seja incluído.

Game::Game() {
    // Inicialização de variáveis básicas
    caios = 0;
    totalCPS = 0.0;
    clickPower = 1.0;
    currentScreen = GAMEPLAY; // Começa no gameplay conforme seu código
    clicks = 0;
    clickAchieve = 0;
    frameCounter = 0;

    // Configuração UI
    uiX = 450.f;
    uiY = 10.f;
    buttonWidth = 312.f;
    buttonHeight = 50.f;
    buttonSpacing = 2.f;
    backButtonRect = { 10, 10, 50, 50 };

    // Inicializa o botão (assumindo tela 800x600, idealmente passaria isso no construtor)
    botaoDoCaio = new ClickerButton({ 400, 300}, 50, 60, RED);
}

Game::~Game() {
    SaveGame(); // Salva ao fechar
    UnloadFont(font);
    delete botaoDoCaio;
}

void Game::Init() {
    // Carrega Fonte
    font = LoadFontEx("m5x7.ttf", 72, 0, 250);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    GuiSetFont(font);

    if (font.texture.id == 0) {
        TraceLog(LOG_WARNING, "Fonte não encontrada. Usando fonte padrão.");
    }
    else {
        TraceLog(LOG_INFO, "Fonte carregada com sucesso!");
    }

    // Inicializa Conteúdo
    InitGameContent();

    // Carrega Save
    LoadGame();
}

void Game::InitGameContent() {
    // Seus push_backs aqui
    // Upgrades
    upgrades.emplace_back("Trufa do sebastião", 100, ClickUpgrade, 2.0f);
    upgrades.emplace_back("Brownie do Kleber", 500, ClickUpgrade, 2.5f);
    // ... adicione todos os outros upgrades ...

    // Itens
    itens.emplace_back("Professora de Química", 0.1, 10);
    itens.emplace_back("Beijo do bubulane", 0.3, 30);
    itens.emplace_back("Cama de casal", 0.5, 50);
    itens.emplace_back("Pão", 1, 100);
}

void Game::SaveGame() {
    FILE* saveFile = fopen("save.dat", "wb");
    if (saveFile == NULL) return;

    fwrite(&caios, sizeof(caios), 1, saveFile);
    fwrite(&clickPower, sizeof(clickPower), 1, saveFile);

    for (size_t i = 0; i < itens.size(); i++) {
        fwrite(&itens[i].count, sizeof(itens[i].count), 1, saveFile);
        fwrite(&itens[i].currentCost, sizeof(itens[i].currentCost), 1, saveFile);
        fwrite(&itens[i].currentMultiplier, sizeof(itens[i].currentMultiplier), 1, saveFile);
    }

    for (int i = 0; i < upgrades.size(); i++) {
        fwrite(&upgrades[i].isPurchased, sizeof(bool), 1, saveFile);
    }

    fclose(saveFile);
}

void Game::LoadGame() {
    FILE* saveFile = fopen("save.dat", "rb");
    if (saveFile == NULL) return;

    fread(&caios, sizeof(caios), 1, saveFile);
    fread(&clickPower, sizeof(clickPower), 1, saveFile);

    // Importante: Os itens já devem ter sido criados pelo InitGameContent
    // para que o loop funcione corretamente e preencha os dados.
    for (size_t i = 0; i < itens.size(); i++) {
        if (i >= itens.size()) break; // Segurança
        fread(&itens[i].count, sizeof(itens[i].count), 1, saveFile);
        fread(&itens[i].currentCost, sizeof(itens[i].currentCost), 1, saveFile);
        fread(&itens[i].currentMultiplier, sizeof(itens[i].currentMultiplier), 1, saveFile);
    }

    for (int i = 0; i < upgrades.size(); i++) {
        // Proteção simples para não quebrar se o save for antigo/menor
        if (feof(saveFile)) break;

        fread(&upgrades[i].isPurchased, sizeof(bool), 1, saveFile);
    }

    fclose(saveFile);
}

void Game::ResetSave() {
    // 1. APAGA O ARQUIVO DO DISCO
    // A função remove() retorna 0 se deletou com sucesso
    if (remove("save.dat") == 0) {
        TraceLog(LOG_INFO, "Save deletado com sucesso.");
    }
    else {
        TraceLog(LOG_WARNING, "Nenhum save para deletar ou erro de acesso.");
    }

    // 2. RESETA AS VARIÁVEIS NA MEMÓRIA
    caios = 0;
    totalCPS = 0;
    clickPower = 1.0;
    clicks = 0;
    clickAchieve = 0;

    // 3. RESETA OS CONTEÚDOS (Itens e Upgrades)
    // Limpa as listas atuais (que estão com preços inflacionados)
    itens.clear();
    upgrades.clear();
    floatingTexts.clear();

    // Recria tudo do zero com os valores originais
    InitGameContent();

    // Opcional: Se quiser salvar o estado "zerado" imediatamente
    SaveGame();
}

void Game::Update() {
    if (IsKeyPressed(KEY_R)) ResetSave();

    // Lógica específica por tela
    switch (currentScreen) {
        case LOGO: UpdateLogo(); break;
        case MENU: UpdateMenu(); break;
        case GAMEPLAY: UpdateGameplay(); break;
        case SHOP: UpdateShop(); break;
        case ACHIEVEMENTS: UpdateAchievments(); break;
    }

    if (currentScreen != LOGO && currentScreen != MENU) {
        UpdateCaios();
    }
}

void Game::UpdateLogo() {
    frameCounter++;
    if (frameCounter > 120) currentScreen = MENU;
}

void Game::UpdateMenu() {

}

void Game::UpdateGameplay() {
    if (botaoDoCaio->Update(GetMousePosition())) {
        caios += clickPower;
        // clicks++; // Variável local no seu main original, promovi para a classe

        // Cria texto flutuante
        std::string textToShow = TextFormat("+%.0f", clickPower);
        floatingTexts.emplace_back(GetMousePosition(), ftextLifetime, textToShow);
    }

    // Atualiza textos flutuantes
    for (int i = floatingTexts.size() - 1; i >= 0; i--) {
        FloatingText& txt = floatingTexts[i];
        txt.lifetime -= GetFrameTime();
        txt.position.y -= 20 * GetFrameTime();

        if (txt.lifetime <= 0) {
            floatingTexts.erase(floatingTexts.begin() + i);
        }
    }
}

void Game::UpdateShop() {

}

void Game::UpdateAchievments() {

}

void Game::UpdateCaios() {
    // Cálculo dos caios (CPS)
    totalCPS = 0;
    for (auto& item : itens) {
        totalCPS += item.GetContribuition();
    }
    caios += totalCPS * GetFrameTime();
}

void Game::Draw() {
    BeginDrawing();

    // Configurações globais de estilo
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(DARKGRAY));

    switch (currentScreen) {
        case LOGO: DrawLogo(); break;
        case MENU: DrawMenu(); break;
        case GAMEPLAY: DrawGameplay(); break;
        case SHOP: DrawShop(); break;
        case ACHIEVEMENTS: DrawAchievements(); break;
    }

    // Desenha HUD (exceto em Logo e Menu)
    if (currentScreen != LOGO && currentScreen != MENU) {
        DrawHUD();
    }

    EndDrawing();
}

// --- Implementação dos métodos Draw específicos ---

void Game::DrawLogo() {
    ClearBackground(BLACK);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiLabel({ 0,0, (float)GetScreenWidth(), (float)GetScreenHeight() }, "GAME BY SAMTLOKOMEMO");
}

void Game::DrawMenu() {
    ClearBackground(DARKGREEN);
    // Título
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel({ 0, 0, (float)GetScreenWidth(), 100 }, "CAIO CLICKER");

    // Botão Jogar
    GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(RED));
    float tamanhoDoTexto = MeasureText("JOGAR", 24);
    if (GuiButton({ GetScreenWidth() / 2 - 60.f, GetScreenHeight() / 4.f, tamanhoDoTexto + 30, 36 }, "JOGAR")) {
        currentScreen = GAMEPLAY;
    }
}

void Game::DrawGameplay() {
    ClearBackground(darkPurple);

    // Título
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel({ 0, 0, (float)GetScreenWidth(), 100 }, "CAIO CLICKER");

    // Botão Loja
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(LIME));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(GREEN));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(DARKGREEN));
    if (GuiButton({ GetScreenWidth() - 75.f, GetScreenHeight() - 75.f, 50.f, 50.f }, "$")) currentScreen = SHOP;

    // Botão Conquistas
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(YELLOW));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(ORANGE));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(RED));
    if (GuiButton({ 25.f, GetScreenHeight() - 75.f, 50.f, 50.f }, "#157#")) currentScreen = ACHIEVEMENTS;

    // Botão Principal
    botaoDoCaio->Draw();

    // Textos Flutuantes
    for (const auto& txt : floatingTexts) {
        float fadeAlpha = txt.lifetime / ftextLifetime;
        Color fadeColor = Fade(WHITE, fadeAlpha);
        DrawText(txt.text.c_str(), txt.position.x, txt.position.y, 20, fadeColor);
    }
}

void Game::DrawShop() {
    ClearBackground(darkPurple);

    // Botão Voltar
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    // ... outros estilos do botão ...
    if (GuiButton(backButtonRect, "<")) currentScreen = GAMEPLAY;

    // Título Loja
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
    GuiLabel({ 100, 20, (float)GetScreenWidth(), (float)GetScreenHeight() }, "LOJA\n\n\n DO\n\n\nCAIO");

    // Upgrades (Grade)
    {
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(SKYBLUE));
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
        // ... estilos upgrade ...

        float upgradeSize = 48.0f;
        float upgradeSpacing = 4.0f;

        int visualIndex = 0;

        for (int i = 0; i < upgrades.size(); i++) {
            Upgrade& upg = upgrades[i];
            if (upg.isPurchased) continue;

            // Posição
            float buttonX = uiX + (visualIndex % 6) * (upgradeSize + upgradeSpacing);
            float buttonY = uiY + (visualIndex / 6) * (upgradeSize + upgradeSpacing);
            Rectangle buttonRect = { buttonX, buttonY, upgradeSize, upgradeSize };

            if (caios < upg.cost) GuiDisable();

            const char* iconText = TextFormat("%c", upg.name[0]);

            // Tooltip (Lógica simplificada, idealmente o raygui lida com isso ou desenhamos manual)
            if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
                // DrawRectangle ou GuiLabel para tooltip customizado se GuiTooltip não atender
                // GuiTooltip(buttonRect);
            }

            if (GuiButton(buttonRect, iconText)) {
                caios -= upg.cost;
                upg.isPurchased = true;
                // Aplica lógica do upgrade...
                if (upg.type == ClickUpgrade) clickPower += upg.multiplier; // Assumindo += baseado no seu código, ou *=
                else if (upg.type == ItemUpgrade) {
                    // Lógica de item upgrade...
                }
            }
            GuiEnable();
            visualIndex++;
        }
    }

    // Itens (Lista)
    {
        float upgradeAreaHeight = 100.0f; // Estimativa, calcule baseado nos upgrades se quiser
        float itemUiY = uiY + upgradeAreaHeight + 20.0f; // Ajuste conforme necessário

        GuiSetStyle(DEFAULT, TEXT_SIZE, 12);
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);

        for (size_t i = 0; i < itens.size(); i++) {
            Item& item = itens[i];
            float itemY = itemUiY + (i * (buttonHeight + buttonSpacing));
            Rectangle buttonRect = { uiX, itemY, buttonWidth, buttonHeight };

            if (caios < item.currentCost) GuiDisable();

            const char* buttonText = TextFormat("%s (%d) | Custo: %.0f", item.name.c_str(), item.count, item.currentCost);

            if (GuiButton(buttonRect, buttonText)) {
                item.Buy(caios);
            }
            GuiEnable();
        }
    }

    // Reseta estilo
    GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
}

void Game::DrawAchievements() {
    ClearBackground(darkPurple);

    // Botão Voltar
    GuiSetStyle(DEFAULT, TEXT_SIZE, 36);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
    // ... estilos ...
    if (GuiButton(backButtonRect, "<")) currentScreen = GAMEPLAY;

    // Grade de Conquistas
    int colunas = 6;
    float achievementSize = 50.0f;
    float achievementSpacing = 30.0f;
    float startX = 175.0f;
    float startY = 100.0f;

    for (int i = 0; i < 30; i++) {
        int linha = i / colunas;
        int col = i % colunas;
        float buttonX = startX + (col * (achievementSize + achievementSpacing));
        float buttonY = startY + (linha * (achievementSize + achievementSpacing));
        Rectangle achRect = { buttonX, buttonY, achievementSize, achievementSize };

        if (GuiButton(achRect, TextFormat("%d", i))) {
            // Click
        }

        if (CheckCollisionPointRec(GetMousePosition(), achRect)) {
            // Tooltip
        }
    }
}

void Game::DrawHUD() {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_MIDDLE);
    GuiLabel({ 0, (float)GetScreenHeight() - 100, (float)GetScreenWidth(), 20 }, TextFormat("você tem %.0f caios", caios));
    GuiLabel({ 0, (float)GetScreenHeight() - 100, (float)GetScreenWidth(), 45 }, TextFormat("você ganha %.1f caios por segundo", totalCPS));
}