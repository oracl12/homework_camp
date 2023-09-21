#include <iostream>
#include "../headers/socket_utils.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#endif

void WSACleanupIfNeeded()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void closeSocket(int socket)
{
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

void WSAStartupIfNeeded()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        exit(1);
    }
#endif
}

int initsSocket()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanupIfNeeded();
        exit(1);
    }
    return clientSocket;
};

sockaddr_in getServerAddr()
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_SERVER);
    return serverAddr;
}

void bindSocket(int socket)
{
    sockaddr_in serverAddr = getServerAddr();

    if (bind(socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error binding socket." << std::endl;
        closeSocket(socket);
        WSACleanupIfNeeded();
        exit(1);
    }
};

void listenToSocket(int socket)
{
    if (listen(socket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        std::cerr << "Error listening for connections." << std::endl;
        closeSocket(socket);
        WSACleanupIfNeeded();
        exit(1);
    }
};

void connectToSocket(int socket)
{
    sockaddr_in serverAddr = getServerAddr();

    if (connect(socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error connecting to the server." << std::endl;
        closeSocket(socket);
        WSACleanupIfNeeded();
        exit(1);
    }
};

char *getIp(sockaddr_in *clientAddr)
{
    char *clientIP = new char[INET_ADDRSTRLEN];
#ifdef _WIN32
    DWORD clientIPStrLen = INET_ADDRSTRLEN;

    if (WSAAddressToStringA((LPSOCKADDR)clientAddr, sizeof(*clientAddr), NULL, clientIP, &clientIPStrLen) != 0)
    {
        std::cerr << "Error converting IP address to string." << std::endl;
        clientIP = "undefined user\n";
    }
#else
    if (inet_ntop(AF_INET, &((*clientAddr).sin_addr), clientIP, INET_ADDRSTRLEN) == NULL)
    {
        std::cerr << "Error converting client address to string." << std::endl;
        clientIP = "undefined user\n";
    }
#endif
    return clientIP;
};