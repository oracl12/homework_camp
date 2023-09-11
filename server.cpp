#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include "headers/socket_utils.h"
#include "headers/other.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <signal.h>
#endif

std::vector<std::thread> clients;
std::mutex clientsGuard;
std::mutex stopConditionGuard;
bool stopAllInstances = false;
int serverSocket;

void removeThread(std::thread::id id)
{
    std::lock_guard<std::mutex> lock(clientsGuard);
    auto iter = std::find_if(clients.begin(), clients.end(), [=](std::thread &t) { return (t.get_id() == id); });
    if (iter != clients.end())
    {
        iter->detach();
        clients.erase(iter);
    }
}

void handleClient(int clientSocket, char* ip)
{
    std::cout << "Client connected." << std::endl;

    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(stopConditionGuard);
            if (stopAllInstances) {
                closeSocket(clientSocket);
                return;
            }
         }

        char buffer[256];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead > 0)
        {
            std::cout << "User " << ip << " --- " << buffer << std::endl;

            const char* response = "Server is alive!";
            ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Error sending response." << std::endl;
            } else {
                std::cout << "Sent response to client." << std::endl;
            }
            continue;
        }
        delete[] ip;
        closeSocket(clientSocket);
        removeThread(std::this_thread::get_id());

        std::cerr << "Client disconnected & thread removed" << std::endl;
        break;
    }
}

void stopAllClientInstances()
{
    std::lock_guard<std::mutex> lock(clientsGuard);
    for (std::thread& instance : clients)
    {
        std::cout << "Joining thread with id "  << instance.get_id() << std::endl;
        instance.join();
    }
    clients.clear();
}

#ifdef _WIN32
BOOL WINAPI ctrlHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
#else
void ctrlHandler(int signal)
{
    if (signal == SIGINT)
    {
#endif
        std::cout << "Ctrl+C received. Stopping all instances..." << std::endl;

        { 
            std::lock_guard<std::mutex> lock(stopConditionGuard);
            stopAllInstances = true;
        }

        if (serverSocket > 0) {
            closeSocket(serverSocket);
        }

        stopAllClientInstances();
        WSACleanupIfNeeded();

        exit(1);
    }
#ifdef _WIN32
    return TRUE;
#endif
}


int main()
{
#ifdef _WIN32
    if (!SetConsoleCtrlHandler(ctrlHandler, TRUE))
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }
#else
    signal(SIGINT, ctrlHandler);
#endif

    std::cout << "STARTING SERVER" << std::endl;

    WSAStartupIfNeeded();

    serverSocket = initsSocket();

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bindSocket(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    listenToSocket(serverSocket, MAX_CLIENTS);

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    while (true)
    {
        sockaddr_in clientAddr;
#ifdef _WIN32
        int clientAddrLen = sizeof(clientAddr);
#else
        socklen_t clientAddrLen = sizeof(clientAddr);
#endif
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

        if (stopAllInstances)
        {
            std::cerr << "Server is shutting down. Rejecting new connections." << std::endl;

            if (clientSocket != INVALID_SOCKET) {
                closeSocket(clientSocket);
            }
            break;
        }

        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Error accepting connection." << std::endl;
            break;
        }

        {
            std::lock_guard<std::mutex> lock(clientsGuard);
            clients.push_back(std::thread(handleClient, clientSocket, getIp(&clientAddr)));
        }
    }

    stopAllClientInstances();
    closeSocket(serverSocket);
    WSACleanupIfNeeded();

    return 0;
}