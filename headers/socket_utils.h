#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 46
#endif

#define SOCKET_ERROR (-1)
#define PORT 5555
#define IP_SERVER "127.0.0.1"
#define MAX_CLIENTS 4

void WSACleanupIfNeeded();

void closeSocket(int socket);

void WSAStartupIfNeeded();

int initsSocket();

sockaddr_in getServerAddr();

void bindSocket(int socket);

void listenToSocket(int socket);

void connectToSocket(int socket);

char *getIp(sockaddr_in *sin_addr);
#endif