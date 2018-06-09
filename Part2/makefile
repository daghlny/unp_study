all: server client

server: server.c
	gcc server.c -o server
client: client.c
	gcc client.c -o client
clean: server client
	rm -f server client http_client
