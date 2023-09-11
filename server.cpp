#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <thread>

#define PORT 5555

int main()
{
    std::cout << "STARTING SERVER" << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error binding socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    while (true)
    {

        std::cout << "LISTENING" << std::endl;

        if (listen(serverSocket, 5) == SOCKET_ERROR)
        {
            std::cerr << "Error listening for connections." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Error accepting connection." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Client connected." << std::endl;

        std::cout << "Server is listening on port" << PORT << "..." << std::endl;

        while (1)
        {
            char buffer[1024];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                std::cout << "Client says: " << buffer << std::endl;
                Sleep(100);
                continue;
            }

            closesocket(clientSocket);
            std::cerr << "Client disconected" << std::endl;
            break;
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}