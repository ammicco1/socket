#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLEN 255
#define OPT_LIST "p:a:s:vh"

static void _quit(char *error){
	fputs(error, stderr);
	
	exit(1);
}

static void _log(char *log){
	fputs(log, stderr);
}

static void _help(){
	fputs("Usage: ./client [-p] - specify port for the server (default is 3000)\n\t\t\
[-a] - specify address for the server (default is localhost)\n\t\t\
[-s] - specify the string to send for echo\n\t\t\
[-v] - show logs\n", stdout);
}

int main(int argc, char **argv){
	int port = 3000, opt, sd, bytes, server_addr_len, sflag = 0, vflag = 0;
	char buff[MAXLEN], *addr = NULL;
	struct sockaddr_in server_addr;

	memset(buff, 0, MAXLEN);

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
	sd = socket(AF_INET, SOCK_STREAM, 0);

	if(sd < 0){
		_quit("socket() error.\n");
	}

	if(vflag){_log("socket() ok.\n");}

	/* set server's address and port */	
	server_addr_len = sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr ? inet_addr(addr) : inet_addr("127.0.0.1");
	server_addr.sin_port = htons(port);

	/* open connection */
	if(connect(sd, (struct sockaddr *) &server_addr, server_addr_len) < 0){
		_quit("connect() error.\n");
	}

	if(vflag){_log("connect() ok.\n");}

	if(!sflag){
		fputs("Insert string: ", stdout);
		fgets(buff, MAXLEN, stdin);
	}

	/* send string to server */
	bytes = write(sd, buff, MAXLEN);

	if(bytes <= 0){
		_quit("write() error.\n");
	}

	if(vflag){fprintf(stderr, "%d byte sent to server.\n", bytes);}

	/* read response from server */	
	bytes = read(sd, buff, MAXLEN);

	if(bytes <= 0){
		_quit("read() error.\n");
	}

	if(vflag){fprintf(stderr, "%d byte received from server.\n", bytes);}
	printf("Echo: %s", buff);

	/* close connection */
	close(sd);	

	return 0;
}