#include <uWebSockets/App.h>
#include <iostream>
#include <string_view>

struct UserData {}; 

void runWebSocketServer() {
    std::cout << "Starting WebSocket server on port 9001..." << std::endl;

    uWS::App().ws<UserData>("/*", {
        .open = [](auto* ws) {
            std::cout << "A client connected!" << std::endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            std::cout << "Received message: " << message << std::endl;
            ws->send(message, opCode); 
        },
        .close = [](auto* ws, int code, std::string_view message) {
            std::cout << "A client disconnected!" << std::endl;
        }
    });
    // .listen(9001, [](auto* listen_socket) {
    //     if (listen_socket) {
    //         std::cout << "Listening on port 9001" << std::endl;
    //     } else {
    //         std::cout << "Failed to listen on port 9001" << std::endl;
    //     }
    // }).run();
}

int main() {
    runWebSocketServer();
    return 0;
}
