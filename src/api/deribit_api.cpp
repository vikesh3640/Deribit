#include "deribit_api.h"
#include "../config/api_config.h"
#include <curl/curl.h>
#include "json.hpp"
#include <iostream>
#include <string>
#include <chrono>

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

DeribitAPI::DeribitAPI() {
    curl_global_init(CURL_GLOBAL_ALL);
}

bool DeribitAPI::authenticate(const std::string& clientId, const std::string& clientSecret) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::string authUrl = "https://test.deribit.com/api/v2/public/auth?client_id=" + clientId +
                          "&client_secret=" + clientSecret + "&grant_type=client_credentials";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, authUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        try {
            json jsonResponse = json::parse(readBuffer);
            if (jsonResponse.contains("error")) {
                std::cerr << "Error in authentication response: " << jsonResponse["error"]["message"] << std::endl;
                return false;
            }
            authToken = jsonResponse["result"]["access_token"];
            
            auto end = std::chrono::high_resolution_clock::now();  // End time measurement
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Authentication took " << duration.count() << " milliseconds.\n";  // Log time taken

            return true;
        } catch (json::parse_error& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            return false;
        }
    }

    std::cerr << "Failed to initialize cURL" << std::endl;
    return false;
}

bool DeribitAPI::placeOrder(Order& order) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    std::cout << "welcome to placing order " << std::endl;
    std::string endpoint = Config::BASE_URL + ((order.buyOrSell == 'b') ? Config::PLACE_ORDER_ENDPOINT : Config::SELL_ORDER_ENDPOINT);

    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", (order.buyOrSell == 'b') ? "private/buy" : "private/sell"},
        {"params", {
            {"instrument_name", order.symbol},
            {"amount", order.quantity},
            {"type", order.type},
            {"price", order.price}
        }}
    };

    if (order.type == "stop_limit" || order.type == "stop_market" ||
        order.type == "trailing_stop" || order.type == "take_limit" || order.type == "take_market") {
        payload["params"]["trigger_price"] = order.trigger_price;
        payload["params"]["trigger"] = order.trigger;
    }

    std::string response = sendRequest(endpoint, payload.dump());

    json jsonResponse = json::parse(response);
    if (jsonResponse.contains("error")) {
        std::cerr << "Error placing order: " << jsonResponse["error"]["code"] << " " << jsonResponse["error"]["data"]["reason"] << std::endl;
        return false;
    }

    order.id = jsonResponse["result"]["order"]["order_id"];
    std::cout << "Order placed successfully. Order ID: " << order.id << std::endl;

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Order placement took " << duration.count() << " milliseconds.\n";  // Log time taken

    return true;
}

bool DeribitAPI::cancelOrder(const std::string& orderId) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    std::string endpoint = Config::BASE_URL + Config::CANCEL_ORDER_ENDPOINT;
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/cancel"},
        {"params", {
            {"order_id", orderId}
        }}
    };

    std::string response = sendRequest(endpoint, payload.dump());
    json jsonResponse = json::parse(response);

    if (jsonResponse.contains("error")) {
        std::cerr << "Error canceling order: " << jsonResponse["error"]["message"] << std::endl;
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Canceling order took " << duration.count() << " milliseconds.\n";  // Log time taken

    return true;
}

bool DeribitAPI::modifyOrder(const std::string& orderId, double newPrice, int newQuantity) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    std::string endpoint = Config::BASE_URL + Config::MODIFY_ORDER_ENDPOINT;
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/edit"},
        {"params", {
            {"order_id", orderId},
            {"amount", newQuantity},
            {"price", newPrice}
        }}
    };

    std::string response = sendRequest(endpoint, payload.dump());
    json jsonResponse = json::parse(response);

    if (jsonResponse.contains("error")) {
        std::cerr << "Error modifying order: " << jsonResponse["error"]["message"] << std::endl;
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Modifying order took " << duration.count() << " milliseconds.\n";  // Log time taken

    return true;
}

Orderbook DeribitAPI::getOrderbook(const std::string& symbol) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string endpoint = Config::BASE_URL + "public/get_order_book?depth=5&instrument_name=" + symbol;
    Orderbook orderbook;

    curl = curl_easy_init();
    if (curl) {
        std::cout << "Sending request to: " << endpoint << std::endl;
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (authToken.empty()) {
            if (!authenticate(Config::API_KEY, Config::API_SECRET)) {
                std::cerr << "Failed to authenticate" << std::endl;
            }
        }

        std::string authHeader = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        json jsonResponse = json::parse(readBuffer);

        if (jsonResponse.contains("error")) {
            std::cerr << "Error retrieving orderbook: " << jsonResponse["error"]["message"] << std::endl;
            return orderbook;
        }

        for (const auto& bid : jsonResponse["result"]["bids"]) {
            orderbook.bids.push_back({bid[0], bid[1]});
        }

        for (const auto& ask : jsonResponse["result"]["asks"]) {
            orderbook.asks.push_back({ask[0], ask[1]});
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Fetching orderbook took " << duration.count() << " milliseconds.\n";  // Log time taken

    return orderbook;
}

void DeribitAPI::getOpenOrders() {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    std::string endpoint = Config::BASE_URL + "private/get_open_orders";
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/get_open_orders"},
        {"params", {}}
    };

    std::string response = sendRequest(endpoint, payload.dump());
    json jsonResponse = json::parse(response);

    if (jsonResponse.contains("error")) {
        std::cerr << "Error retrieving open orders: " << jsonResponse["error"]["message"] << std::endl;
        return;
    }

    for (auto it : jsonResponse["result"]) {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "Order ID:     " << it["order_id"] << std::endl;
        std::cout << "Symbol:       " << it["instrument_name"] << std::endl;
        std::cout << "Price:        " << it["price"] << std::endl;
        std::cout << "Quantity:     " << it["amount"] << std::endl;
        std::cout << "Status:       " << it["order_status"] << std::endl;
        std::cout << "Type:         " << it["order_type"] << std::endl;
        std::cout << "Buy/Sell:     " << it["direction"] << std::endl;
    }
    std::cout << "-----------------------------------" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Fetching open orders took " << duration.count() << " milliseconds.\n";  // Log time taken
}

Position DeribitAPI::getPosition(const std::string& symbol) {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    Position position;

    std::string endpoint = Config::BASE_URL + Config::GET_POSITIONS_ENDPOINT + "?instrument_name=" + symbol;

    curl = curl_easy_init();
    if (curl) {
        std::cout << "Sending request to: " << endpoint << std::endl;
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (authToken.empty()) {
            if (!authenticate(Config::API_KEY, Config::API_SECRET)) {
                std::cerr << "Failed to authenticate" << std::endl;
                return Position();
            }
        }

        std::string authHeader = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);
        json jsonResponse = json::parse(readBuffer);

        if (jsonResponse.contains("error")) {
            std::cerr << "Error retrieving positions: " << jsonResponse["error"]["message"] << std::endl;
            return position;
        }

        if (!jsonResponse["result"].empty()) {
            const auto& pos = jsonResponse["result"][0];
            position.symbol = pos["instrument_name"];
            position.quantity = pos["size"];
            position.currentPrice = pos["mark_price"];
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Fetching positions took " << duration.count() << " milliseconds.\n";  // Log time taken

    return position;
}

void DeribitAPI::getCurrentPositions() {
    auto start = std::chrono::high_resolution_clock::now();  // Start time measurement
    std::string endpoint = Config::BASE_URL + "private/get_positions";
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/get_positions"},
        {"params", {
            {"currency", "BTC"},  // You can modify "BTC" if needed
            {"kind", "future"},   // "future", "option" or "spot"
            {"type", "all"}
        }}
    };

    std::string response = sendRequest(endpoint, payload.dump());
    json jsonResponse = json::parse(response);

    if (jsonResponse.contains("error")) {
        std::cerr << "Error retrieving current positions: " << jsonResponse["error"]["message"] << std::endl;
        return;
    }

    for (const auto& pos : jsonResponse["result"]) {
        std::cout << "Symbol: " << pos["instrument_name"] << ", Size: " << pos["size"] << ", Avg Price: " << pos["average_price"] << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();  // End time measurement
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Fetching current positions took " << duration.count() << " milliseconds.\n";  // Log time taken
}

std::string DeribitAPI::sendRequest(const std::string& endpoint, const std::string& payload) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::cout << "Sending request to: " << endpoint << std::endl;
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (authToken.empty()) {
            if (!authenticate(Config::API_KEY, Config::API_SECRET)) {
                std::cerr << "Failed to authenticate" << std::endl;
                return "";
            }
        }

        std::string authHeader = "Authorization: Bearer " + authToken;
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return "";
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return readBuffer;
}
