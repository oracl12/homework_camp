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

int main()
{
    std::cout << "STARTING CLIENT" << std::endl;

    WSAStartupIfNeeded();

    int clientSocket = initsSocket();

    connectToSocket(clientSocket);

    std::cout << "Connected to the server." << std::endl;

    while (true)
    {
        std::cout << "SENDING DATA" << std::endl;
        SystemInfo sysInfo;
        std::string info = sysInfo.getUnifiedData();

        int bytesSent = send(clientSocket, info.c_str(), info.size(), 0);

        if (bytesSent <= 0)
        {
            std::cerr << "Server is dead" << std::endl;
            break;
        }

        char buffer[128];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            std::cerr << "Server is dead" << std::endl;
            break;
        }
        else
        {
            std::cout << "Success." << std::endl;
        }

        sleepForFiveSeconds();
    }

    closeSocket(clientSocket);
    WSACleanupIfNeeded();

    return 0;
}