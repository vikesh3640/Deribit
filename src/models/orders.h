#pragma once

#include<string>

struct Order {
    std::string id;
    std::string symbol;
    int quantity;
    int price;
    std::string trigger;
    std::string type;
    std::string status;
    char buyOrSell;
    int trigger_price;

    Order(const std::string& symbol, int price, int quantity, int trigger_price, const char& buyOrSell = 'b',  const std::string&trigger = "last_trigger", const std::string& type = "limit", const std::string& status = "open")
        : id(""), symbol(symbol), price(price), quantity(quantity), type(type), status(status), buyOrSell(buyOrSell), trigger_price(trigger_price), trigger(trigger) {}

};