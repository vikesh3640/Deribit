# Order Execution and Management System for Deribit Test

## Objective
A high-performance C++ system for order execution and management on the Deribit Test platform. The system enables seamless trading of Spot, Futures, and Options with real-time market data updates and comprehensive order management functionalities.

---

## Features

### Order Management
- **Place Order**: Execute new trades on supported instruments.
- **Cancel Order**: Cancel open orders.
- **Modify Order**: Edit existing orders.
- **Get Orderbook**: Fetch the current orderbook data for selected symbols.
- **View Current Positions**: Display active positions for all instruments.

### Real-Time Market Data
- **WebSocket Integration**:
  - Real-time streaming of market data.
  - WebSocket server for client subscriptions to symbols.
  - Continuous orderbook updates for subscribed symbols.

### Market Coverage
- **Instruments**: Spot, Futures, and Options.
- **Scope**: All supported symbols on the Deribit Test platform.

---

## Project Structure

```plaintext
ORDER-EXECUTION-AND-MANAGEMENT  
│  
├── .vscode/  
│   ├── c_cpp_properties.json   # Configuration for VS Code C++ settings  
│   ├── tasks.json              # Task automation for building  
│  
├── src/  
│   ├── api/  
│   │   ├── deribit_api.cpp     // API integration for order execution  
│   │   ├── deribit_api.h       // Header file for API functions  
│   │   ├── json.hpp            // JSON handling for API responses  
│   │  
│   ├── config/  
│   │   ├── api_config.h        // Configuration for API keys and settings  
│   │  
│   ├── models/  
│       ├── orderbook.h         // Orderbook data model  
│       ├── orders.h            // Order details and handling  
│       ├── position.h          // Model for tracking positions  
│  
├── main.cpp                    // Main entry point of the application  
├── main.exe                    // Compiled executable  
├── main                        // Other output artifacts  
```
## Setup Instructions

### Prerequisites
- **A C++ compiler** supporting C++17 (e.g., GCC, Clang, or MSVC).
- **An active account** on Deribit Test.

### Steps to Run

1. **Clone the Repository**  
   Clone this repository to your local machine:
   ```bash
   git clone <repository-url>
   cd ORDER-EXECUTION-AND-MANAGEMENT
2. **Install Dependencies**
Ensure your system has the necessary libraries installed for JSON parsing and WebSocket integration.

3. **Configure API Keys**
Open the file src/config/api_config.h and add your client ID and client secret generated from Deribit Test.
Example:
```bash
#define CLIENT_ID "your_client_id"
#define CLIENT_SECRET "your_client_secret"
```
4. **Compile the Project**
Use the following command to build the project:
```bash
g++ -std=c++17 src/**/*.cpp -o main.exe
```

5. **Run the Application**
Execute the compiled file:
```bash
    ./main.exe
```
