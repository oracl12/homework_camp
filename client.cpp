#include <iostream>
#include <cstring>
#include "headers/info.h"
#include "headers/socket_utils.h"
#include "headers/other.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#endif

int main() {
    std::cout << "STARTING CLIENT" << std::endl;

    WSAStartupIfNeeded();

    int clientSocket = initsSocket();

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_SERVER);

    connectToSocket(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    std::cout << "Connected to the server." << std::endl;

    while (true) {
        std::cout << "SENDING DATA" << std::endl;
        char* info = getUnifiedData();

        if (info == nullptr) {
            info = "SMTH WENT WRONG";
        }

        int bytesSent = send(clientSocket, info, strlen(info), 0);
        delete[] info;

        if (bytesSent <= 0) {
            std::cerr << "Server is dead" << std::endl;
            break;
        }

        char buffer[128];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Server is dead" << std::endl;
            break;
        } else {
            std::cout << "Success." << std::endl;
        }

        sleepForFiveSeconds();
    }

    closeSocket(clientSocket);
    WSACleanupIfNeeded();

    return 0;
}