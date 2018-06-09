all: server client

server: server.cpp
	g++ -std=c++11 server.cpp -o server -lpthread
client: client.cpp
	gcc client.cpp -o client
clean: server client
	rm -f server client
