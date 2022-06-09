#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLEN 255
#define PORT 3333
#define MAXCONN 5

void die(char *);

int main(int argc, char **argv){
	char sendb[MAXLEN], recvb[MAXLEN];
	struct sockaddr_in bind_ip_port, client_ip_port;
	int sd, conn_sd, pid, byterec, bytesend, bind_ip_port_length = sizeof(bind_ip_port), client_ip_port_length = sizeof(client_ip_port), counter = 0;

	memset(sendb, 0, MAXLEN);
	memset(recvb, 0, MAXLEN);

	/* creo la socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sd < 0){
		die("socket() error");
	}

	printf("socket() ok.\n");
	
	/* inserisco i dati del server che devo creare */
	bind_ip_port.sin_family = AF_INET;
	bind_ip_port.sin_addr.s_addr = inet_addr("127.0.0.1");
	bind_ip_port.sin_port = htons(PORT);
	
	/* faccio il bind dell'indirizzo e la porta */
	if(bind(sd, (struct sockaddr *) &bind_ip_port, bind_ip_port_length) < 0){
		die("bind() error");
	}
	
	printf("bind() ok.\n");

	/* do il numero massimo di connessioni */
	if(listen(sd, MAXCONN) != 0){
		die("listen() error");
	}

	printf("listen() ok.\n");

	/* in un ciclo gestisco i client */
	while(1){
		/* accetto la connessione da un client e inizio il 3hs */
		conn_sd = accept(sd, (struct sockaddr *) &client_ip_port, &client_ip_port_length);

		if(conn_sd < 0){
			die("accept() error");
		}

		printf("accept() ok.\n");
		counter++;

		pid = fork();

		if(pid == 0){
			/* processo figlio che gestisce tutte le connessioni accettate dal processo padre */
			close(sd);

			printf("child process handle connection no.: %d\n", counter);

			/* leggo i dati mandati dal client */
			byterec = read(conn_sd, recvb, MAXLEN);

			if(byterec <= 0){
				die("read() error");
			}

			printf("read() ok.\n%d byte received: %s\n", byterec, recvb);
		
			strcpy(sendb, recvb);

			/* spedisco i dati al client */
			bytesend = write(conn_sd, sendb, MAXLEN);

			if(bytesend <= 0){
				die("send() error");
			}

			printf("send() ok.\n");

			close(conn_sd);

			exit(0);
		}else if(pid > 0){
			/* processo padre, che chiude chiude il descriptor della connessione accettata, così da poter tornare ad accettare quelle che arrivano in seguito */
			close(conn_sd);
		}else{
			die("fork() error");
		}
	}
	
	/* chiudo la socket */
	close(sd);
	
	return 0;	
}

void die(char *error){
	fprintf(stderr, "%s.\n", error);
	
	exit(1);
}