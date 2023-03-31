#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFF_LEN 255
#define MAX_CLIENT 5
#define OPT_LIST "p:a:vh"

static void _quit(char *error){
	fputs(error, stderr);
	
	exit(1);
}

static void _log(char *log){
	fputs(log, stderr);
}

static void _help(){
	fputs("Usage: ./server \n\
Options: \n\t\
[-p] - specify port to bind (default is 3000)\n\t\
[-a] - specify address to bind (default is localhost)\n\t\
[-v] - show logs\n\t\
[-h] - show this help\n", stdout);
}

int main(int argc, char **argv){
	int port = 3000, server_sd, client_sd, bytes, server_addr_len, client_addr_len, 
	counter = 0,
	pid,
	opt, vflag = 0;
	char buff[MAX_BUFF_LEN], *addr = NULL;
	struct sockaddr_in server_addr, client_addr;

	memset(buff, 0, MAX_BUFF_LEN);

	while((opt = getopt(argc, argv, OPT_LIST)) != -1){
		switch(opt){
			case 'p': port = atoi(optarg); break; 
			case 'a': addr = optarg; break;
			case 'v': vflag = 1; break;
			case 'h': _help(); exit(0);
		}
	}

	/* create server socket */
	server_sd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_sd < 0){
		_quit("socket() error.\n");
	}

	if(vflag){_log("socket() ok.\n");}
	
	/* set server's address and port */
	server_addr_len 			= sizeof(server_addr);
	server_addr.sin_family 		= AF_INET;
	server_addr.sin_addr.s_addr = addr ? inet_addr(addr) : inet_addr("127.0.0.1");
	server_addr.sin_port 		= htons(port);
	
	/* bind address and port */
	if(bind(server_sd, (struct sockaddr *) &server_addr, server_addr_len) < 0){
		_quit("bind() error.\n");
	}
	
	if(vflag){_log("bind() ok.\n");}

	/* set max number of connections */
	if(listen(server_sd, MAX_CLIENT) != 0){
		_quit("listen() error.\n");
	}

	if(vflag){_log("listen() ok.\n");}

	/* infinite loop for handle clients */
	while(1){
		/* accept client connection */
		client_addr_len = sizeof(client_addr);
		client_sd = accept(server_sd, (struct sockaddr *) &client_addr, (socklen_t *)  &client_addr_len);

		if(client_sd < 0){
			_quit("accept() error.\n");
		}

		if(vflag){_log("accept() ok.\n");}
		
		/* count connection number */
		counter++;

		/* fork a child process for handle more client simultaneously */
		pid = fork();

		if(pid == 0){
			/* child process close server socket and handle client */
			close(server_sd);

			if(vflag){fprintf(stderr, "child process handle connection no.: %d.\n", counter);}

			/* read from client */
			bytes = read(client_sd, buff, MAX_BUFF_LEN);

			if(bytes <= 0){
				_quit("read() error");
			}

			if(vflag){fprintf(stderr, "%d bytes received from client no. %d: %s", bytes, counter, buff);}
		
			/* copy message from the client and echo */
			bytes = write(client_sd, buff, MAX_BUFF_LEN);

			if(bytes <= 0){
				_quit("send() error");
			}

			if(vflag){fprintf(stderr, "%d bytes sent to client no. %d\n", bytes, counter);}

			/* close client socket and return from child process */
			close(client_sd);
			exit(0);
		}else if(pid > 0){
			/* parent process close client socket and return to accept connections */
			close(client_sd);
		}else{
			_quit("fork() error");
		}
	}
	
	/* close server socket */
	close(server_sd);
	
	return 0;	
}