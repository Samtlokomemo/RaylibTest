#pragma once
#include <string>
#include <iostream> 

enum upgradeType {
    ClickUpgrade,
    ItemUpgrade
};

struct Upgrade {
    std::string name;
    double cost;
    upgradeType type;
    float multiplier;
    int targetItemIndex = -1;
    bool isPurchased = false;

    Upgrade(std::string n, double c, upgradeType t, float m, int target = -1);

};