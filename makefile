ARGS = -Wall -ansi -pedantic

client: client.c
	gcc ${ARGS} -o client client.c

server: server.c
	gcc ${ARGS} -o server server.c

clean: 
	rm client server