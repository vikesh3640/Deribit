#include <iostream>
#include "api/deribit_api.h"
#include "config/api_config.h"
#include <string>
#include <cctype>

void placeOrder(DeribitAPI& api) {
    std::string symbol, type, trigger;
    double price;
    int quantity;
    char buyOrSell;
    int trigger_price;

    std::cout << "Buy or Sell? (b/s): ";
    std::cin >> buyOrSell;
    std::cout << "Enter symbol (e.g., BTC-PERPETUAL): ";
    std::cin >> symbol;
    std::cout << "Enter quantity: ";
    std::cin >> quantity;
    std::cout << "Enter type : ";
    std::cin >> type;
    
    // if(type == "limit" or type == "stop_limit") {
        std::cout << "Enter price: ";
        std::cin >> price;
    // }
    
    if(buyOrSell == 's' or type == "stop_limit" or type == "stop_market" or type =="take_limit" or type == "take_market" or type == "trailing_market") {
        std::cout << "Enter trigger price: ";
        std::cin >> trigger_price;
        std::cout << "Enter trigger(last_price/index_price/mark_price): ";
        std::cin >> trigger;
    }
    

    for(auto &c: symbol) c = toupper(c);
    Order order(symbol, price, quantity, trigger_price, buyOrSell, trigger, type);
    if (api.placeOrder(order)) {
    } else {
        std::cout << "Failed to place order.\n";
    }
}

void cancelOrder(DeribitAPI& api) {
    std::string orderId;
    std::cout << "Enter Order ID to cancel: ";
    std::cin >> orderId;

    if (api.cancelOrder(orderId)) {
        std::cout << "Order canceled successfully for orderid: " << orderId << "\n";
    } else {
        std::cout << "Failed to cancel order.\n";
    }
}

void modifyOrder(DeribitAPI& api) {
    std::string orderId;
    double newPrice;
    int newQuantity;

    std::cout << "Enter Order ID to modify: ";
    std::cin >> orderId;
    std::cout << "Enter new price: ";
    std::cin >> newPrice;
    std::cout << "Enter new quantity: ";
    std::cin >> newQuantity;

    if (api.modifyOrder(orderId, newPrice, newQuantity)) {
        std::cout << "Order modified successfully for orderid: " << orderId << "\n";
    } else {
        std::cout << "Failed to modify order.\n";
    }
}

void getOrderbook(DeribitAPI& api) {
    std::string symbol;
    std::cout << "Enter symbol (e.g., BTC-PERPETUAL): ";
    std::cin >> symbol;

    for(auto &c: symbol) c = toupper(c);
    Orderbook orderbook = api.getOrderbook(symbol);
    std::cout << "Orderbook for " << symbol << ":\n";
    
    std::cout << "-----------------------------------" << std::endl;
    for (const auto& bid : orderbook.bids) {
        std::cout << "Bid: Price=   " << bid.price << ",    Quantity=  " << bid.quantity << "\n";
    }
    std::cout << "-----------------------------------" << std::endl;
    for (const auto& ask : orderbook.asks) {
        std::cout << "Ask: Price=   " << ask.price << ",    Quantity=  " << ask.quantity << "\n";
    }
    std::cout << "-----------------------------------" << std::endl;
    
}

void viewPosition(DeribitAPI& api) {
    std::string symbol;
    std::cout << "Enter symbol (e.g., BTC-PERPETUAL): ";
    std::cin >> symbol;

    for(auto &c: symbol) c = toupper(c);
    Position position = api.getPosition(symbol);
    std::cout << "Position for " << symbol << ":\n";
    std::cout << "Size: " << position.quantity << "\n";
    std::cout << "Average Price: " << position.currentPrice << "\n";
}

void viewOpenOrders(DeribitAPI& api) {
    api.getOpenOrders();
}

int main() {
    DeribitAPI api;

    while (true) {
        std::cout << "\nDeribit Order Management System\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Cancel Order\n";
        std::cout << "3. Modify Order\n";
        std::cout << "4. Get Orderbook\n";
        std::cout << "5. View Position\n";
        std::cout << "6. See Open Orders\n";
        std::cout << "7. Exit\n";
        std::cout << "Select an option: ";
        
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                placeOrder(api);
                break;
            case 2:
                cancelOrder(api);
                break;
            case 3:
                modifyOrder(api);
                break;
            case 4:
                getOrderbook(api);
                break;
            case 5:
                viewPosition(api);
                break;
            case 6:
                viewOpenOrders(api);
                break;
            case 7:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    }
}

