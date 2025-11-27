#include <vector>

#include "Upgrade.h"
#include "Item.h"

std::vector<Upgrade> upgrades;
std::vector<Item> itens;

void InitGameContent() {
    // Upgrades
    upgrades.emplace_back("Trufa do sebastiao", 100, ClickUpgrade, 2.0f);
    upgrades.emplace_back("Trufa do sebastiao", 100, ClickUpgrade, 2.0f);
    upgrades.emplace_back("Brownie do Kleber", 500, ClickUpgrade, 2.5f);
    upgrades.emplace_back("Almoco do CDR", 1000, ClickUpgrade, 10.0f);
    upgrades.emplace_back("Sonho com os crias", 3000, ItemUpgrade, 3.0f, 2);

    // Itens
    itens.emplace_back("Professora de Quimica", 0.1, 10);
    itens.emplace_back("Beijo do bubulane", 0.3, 30);
    itens.emplace_back("Cama de casal", 0.5, 50);
    itens.emplace_back("Pao", 1, 100);
}