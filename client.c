#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFF_LEN 255
#define OPT_LIST "p:a:s:vh"

static void _quit(char *error){
	fputs(error, stderr);
	
	exit(1);
}

static void _log(char *log){
	fputs(log, stderr);
}

static void _help(){
	fputs("Usage: ./client\n\
Options: \n\t\
[-p] - specify port for the server (default is 3000)\n\t\
[-a] - specify address for the server (default is localhost)\n\t\
[-s] - specify the string to send for echo\n\t\
[-v] - show logs\n\t\
[-h] - show this help\n", stdout);
}

int main(int argc, char **argv){
	int port = 3000, server_sd, server_addr_len, 
	bytes,
	opt, sflag = 0, vflag = 0;
	char buff[MAX_BUFF_LEN], *addr = NULL;
	struct sockaddr_in server_addr;

	memset(buff, 0, MAX_BUFF_LEN);

	while((opt = getopt(argc, argv, OPT_LIST)) != -1){
	   	switch(opt){
			case 'p': port = atoi(optarg); break;
			case 'a': addr = optarg; break;
			case 's': sflag = 1; strcpy(buff, optarg); strcat(buff, "\n"); break;
			case 'v': vflag = 1; break;
			case 'h': _help(); exit(0);
	   	}
	}
	
	/* create socket */
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

	/* open connection */
	if(connect(server_sd, (struct sockaddr *) &server_addr, server_addr_len) < 0){
		_quit("connect() error.\n");
	}

	if(vflag){_log("connect() ok.\n");}

	if(!sflag){
		fputs("Insert string: ", stdout);
		fgets(buff, MAX_BUFF_LEN, stdin);
	}

	/* send string to server */
	bytes = write(server_sd, buff, MAX_BUFF_LEN);

	if(bytes <= 0){
		_quit("write() error.\n");
	}

	if(vflag){fprintf(stderr, "%d bytes sent to server.\n", bytes);}

	/* read response from server */	
	bytes = read(server_sd, buff, MAX_BUFF_LEN);

	if(bytes <= 0){
		_quit("read() error.\n");
	}

	if(vflag){fprintf(stderr, "%d bytes received from server.\n", bytes);}
	printf("Echo: %s", buff);

	/* close connection */
	close(server_sd);	

	return 0;
}