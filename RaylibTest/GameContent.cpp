#include "GameContent.h"


std::vector<Upgrade> upgrades;
std::vector<Item> itens;

void InitGameContent() {
    // Upgrades
    upgrades.emplace_back("Trufa do sebastião", 100, ClickUpgrade, 2.0f);
    upgrades.emplace_back("Trufa do sebastião", 100, ClickUpgrade, 2.0f);
    upgrades.emplace_back("Brownie do Kleber", 500, ClickUpgrade, 2.5f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Sonho com os crias", 3000, ItemUpgrade, 3.0f, 2);

    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Almoço do CDR", 1000, ClickUpgrade, 10.0f);

    // Itens

    itens.emplace_back("Professora de Química", 0.1, 10);
    itens.emplace_back("Beijo do bubulane", 0.3, 30);
    itens.emplace_back("Cama de casal", 0.5, 50);
    itens.emplace_back("Pão", 1, 100);
}