CC := g++
CFLAGS := -lws2_32 -std=c++11

all: client server

client: client.cpp utils.cpp
	$(CC) $^ -o $@ $(CFLAGS)

server: server.cpp
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	del client.exe
	del server.exe