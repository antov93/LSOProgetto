/*
 ============================================================================
 Name        : Client.c
 Author      : Antonio Vanacore
 Version     :
 Copyright   : Your copyright notice
 Description : Uso dei Socket TCP in C
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>

#define PORTA 1205

int main(void) {

	struct sockaddr_in serverDescriptor; //strut in cui inserisco le info del server a cui voglio connettermi
										 //poiche il client per connettersi deve conoscere ip e porta del server


	int socketClientDescriptor; //descrittore del socket locale del client

	//struttura alla quale mi connetto(il server cioè)
	serverDescriptor.sin_family=AF_INET;
	serverDescriptor.sin_port=htons(PORTA);
	inet_aton("192.168.1.7", &serverDescriptor.sin_addr);

	//creo un socket locale per il client
	socketClientDescriptor=socket(AF_INET,SOCK_STREAM,0);
	if(socketClientDescriptor<0)perror("Errore creazione client socket."),exit(0);

	//connetto il socket client all indirizzo ip del serve
	if( connect(socketClientDescriptor,(struct sockaddr *)&serverDescriptor,sizeof(serverDescriptor)) <0)
		perror("Errore durante la connessione al server."),exit(0);
	printf("!!!!!!Connesso al Server!!!!!!\n");

	char messaggio[100];
	printf("scrivi un messaggio da inviare al server: ");
	scanf("%s",messaggio);

	write(socketClientDescriptor,messaggio,sizeof(messaggio));

	close(socketClientDescriptor);


	return 0;
}
