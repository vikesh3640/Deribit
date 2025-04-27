#pragma once

#include "../models/orderbook.h"
#include "../models/orders.h"
#include "../models/position.h"
#include "../config/api_config.h"
#include <string>
#include <chrono>
#include <iostream>

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
    void getCurrentPositions();
    
    // Timing helpers
    void logTimeTaken(const std::string& action, const std::chrono::high_resolution_clock::time_point& start);
};

// Timing logging function (to be used in method implementations)
inline void DeribitAPI::logTimeTaken(const std::string& action, const std::chrono::high_resolution_clock::time_point& start) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << action << " took " << duration.count() << " milliseconds.\n";
}
