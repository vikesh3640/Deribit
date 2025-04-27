#pragma once
#include <string>

struct Position {
    std::string symbol;
    int quantity;
    double entryPrice;
    double currentPrice;
};