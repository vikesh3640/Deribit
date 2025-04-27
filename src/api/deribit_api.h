#pragma once

#include "../models/orderbook.h"
#include "../models/orders.h"
#include "../models/position.h"
#include "../config/api_config.h"
#include <string>

class DeribitAPI {
    std::string sendRequest(const std::string &endpoint, const std::string& payload);
    bool authenticate(const std::string& clientId, const std::string& clientSecret);

public:
    std::string authToken;
    DeribitAPI();

    bool placeOrder(Order &order);
    bool cancelOrder(const std::string &orderId);
    bool modifyOrder(const std::string &orderId, double price, int quantity);
    Orderbook getOrderbook(const std::string &symbol);
    Position getPosition(const std::string &symbol);
    void getOpenOrders();
};
