#pragma once
#include<vector>

struct OrderbookEntry {
    double price;
    int quantity;
};

struct Orderbook{
    std::vector<OrderbookEntry> bids;
    std::vector<OrderbookEntry> asks;
};

