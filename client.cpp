#include <iostream>
#include <winsock2.h>
#include <thread>
#include <cstring>
#include "utils.h"

#define PORT 5555

int main() {
    std::cout << "STARTING CLIENT" << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to the server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    while (true) {
        std::cout << "SENDING" << std::endl;
        char* info = getUnifiedData();

        if (info == nullptr){
            info = "SMTH WENT WRONG";
        }

        int bytesSent = send(clientSocket, info, strlen(info), 0);

        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Server is dead" << std::endl;
            break;
        }
        delete info;
        Sleep(5000);
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}



// #include <atomic>

// // Global atomic flag to control the loop
// std::atomic<bool> stopLoop(false);

// // Function that runs in the separate thread
// void threadFunction() {
//     std::cout << "Press 'q' and Enter to stop the loop..." << std::endl;
//     while (std::cin.get() != 'q') {
//         // Wait for the user to press 'q'
//     }
//     std::cout << "Stopping the loop..." << std::endl;
//     stopLoop.store(true);
// }

// int main() {
//     // Create and start the separate thread
//     std::thread t(threadFunction);

//     // Main loop
//     while (!stopLoop.load()) {
//         // Your main loop logic here
//     }

//     // Join the separate thread
//     t.join();

//     std::cout << "Main thread exiting." << std::endl;

//     return 0;
// }