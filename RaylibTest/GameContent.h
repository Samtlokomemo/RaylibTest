#pragma once
#include <vector>

#include "Upgrade.h"
#include "Item.h"

class GameContent
{
	std::vector<Upgrade> upgrades;
	std::vector<Item> itens;
	void InitGameContent();
};

