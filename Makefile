CC := g++

ifeq ($(OS),Windows_NT)
CFLAGS := -lws2_32 -std=c++11
else
CFLAGS := -std=c++11 -pthread
endif

DEBUG_CFLAGS := -g

all: client server

client: client.cpp info.cpp socket_utils.cpp other.cpp
	$(CC) $^ -o $@ $(CFLAGS)

server: server.cpp socket_utils.cpp other.cpp
	$(CC) $^ -o $@ $(CFLAGS)

debug: client_debug server_debug

client_debug: client.cpp info.cpp socket_utils.cpp other.cpp
	$(CC) $(DEBUG_CFLAGS) $^ -o $@ $(CFLAGS)

server_debug: server.cpp socket_utils.cpp other.cpp
	$(CC) $(DEBUG_CFLAGS) $^ -o $@ $(CFLAGS)

clean:
ifeq ($(OS),Windows_NT)
	del client.exe
	del client_debug.exe
	del server.exe
	del server_debug.exe
else
	rm -f client
	rm -f server
	rm -f client_debug
	rm -f server_debug
endif

