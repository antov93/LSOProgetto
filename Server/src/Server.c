/*
 ============================================================================
 Name        : Server.c
 Author      : Antonio Vanacore
 Version     :
 Copyright   : Your copyright notice
 Description : Uso dei Socket TCP in C
 ============================================================================
 */
//ciao
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>

#define PORTA 1205 //porta sulla quale il server è in ascolto

int main(int argc, char **argv) {

	struct sockaddr_in indirizzoServer; //serve per mettersi in ascolto su una determinata porta
	struct sockaddr_in indirizzoClient; //serve per contenere le informzioni sulla connessione che mi arrivano dal client
	int socketDescriptor; //informazioni del server
	int clientConnectionDescriptor; //info sul client arrivate tramite l ACCEPT
	int ciao;
	//creo il socket
	socketDescriptor=socket(AF_INET,SOCK_STREAM,0);//creo il socket(famiglia,tipo,protocollo(zero per protocollo migliore disp))
	if(socketDescriptor<0)perror("Errore creazione del socket!"),exit(1);

	indirizzoServer.sin_family=AF_INET;
	indirizzoServer.sin_addr.s_addr=htonl(INADDR_ANY);
	indirizzoServer.sin_port=htons(PORTA);

	//collego il socketDescriptor alla struttura indirizzoServer in cui ci sono i dati per comunicare
	if(bind(socketDescriptor,(struct sockaddr *)&indirizzoServer,sizeof(indirizzoServer))<0)
		perror("Errore BIND:"),exit(0);

	printf("In attesa di richieste da parte di un client...\n");

	//metto in ascolto il socket(sd,lunghezzaCoda);
	listen(socketDescriptor,1);


	while(1){
		//accetto connessioni
		int lunghezzaClient;
		lunghezzaClient=sizeof(indirizzoClient);
		clientConnectionDescriptor=accept(socketDescriptor,(struct sockaddr *)&indirizzoClient,&lunghezzaClient);
		if(clientConnectionDescriptor<0){
			perror("Errore ACCEPT del server");
			exit(0);
		}

		//server concorrente
		pid_t pid;

		//creo un processo figlio
		if((pid=fork())<0){
			perror("Errore nella FORK:");
			exit(0);
		}else if(pid==0){//questo è quello che fa il processo figlio
			close(socketDescriptor); //lavora solo con clientConnectionDescriptor

			printf("*Nuova connessione dal client: %s\n",inet_ntoa(indirizzoClient.sin_addr));
			printf("In attesta di ricezione di un messaggio...\n");

			char buffer[100];
			read(clientConnectionDescriptor,buffer,sizeof(buffer));
			printf("-[%s]Messaggio ricevuto: %s\n",inet_ntoa(indirizzoClient.sin_addr),buffer);
			printf("Attendo altre richieste...\n");

			exit(0);
		 }

		close(clientConnectionDescriptor);


	}//fine while

	return 0;

}
