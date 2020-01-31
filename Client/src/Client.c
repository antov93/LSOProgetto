/*
 ============================================================================
 Name        : Client.c
 Author      : Antonio Vanacore & Valerio Panzera
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
#include <string.h>

#define PORTA 1205
#define MAX 10 

void stampa(char m[][MAX]);

int main(void) {
	int i;
	struct sockaddr_in serverDescriptor; //struct in cui inserisco le info del server a cui voglio connettermi
										 //poiche il client per connettersi deve conoscere ip e porta del server

    char buffer[1000];
	int socketClientDescriptor; //descrittore del socket locale del client
    
	//struttura alla quale mi connetto(il server cioè)
	serverDescriptor.sin_family=AF_INET;
	serverDescriptor.sin_port=htons(PORTA);
	inet_aton("192.168.1.11", &serverDescriptor.sin_addr);
    
	//creo un socket locale per il client
	socketClientDescriptor=socket(AF_INET,SOCK_STREAM,0);
	if(socketClientDescriptor<0)perror("Errore creazione client socket."),exit(0);
    
	//connetto il socket client all indirizzo ip del serve
	if( connect(socketClientDescriptor,(struct sockaddr *)&serverDescriptor,sizeof(serverDescriptor)) <0)
		perror("Errore durante la connessione al server."),exit(0);
	
	printf("!!!!!!Connesso al Server!!!!!!\n");
	printf("\nBENVENUTO NEL GIOCO\n");
	printf("Istruzioni:\nBisogna muoversi nel labirinto, prendere il pacchetto ed uscire per completare il gioco.\n\nLegenda: \nA-personaggio\nE-Uscita\nI-Spazio libero\nO-Ostacolo\nP-Pacchetto preso\n");
	printf("\nUtilizzare i tasti W A S D della tastiera per muovere il personaggio all'interno del labirinto.\n");
	
	//leggo la matrice del gioco e visualizzo il quadro iniziale
		printf("Quadro iniziale!\n");
		read(socketClientDescriptor,buffer,sizeof(buffer));
		
		//stampo la matrice
		for(i=0; i<100; i++){
		        if(i % 10 == 0){
		            printf("\n");     
		        }else{
		                printf("%c ", buffer[i]);
		              }
		    }
		
	while(1){
	char messaggio[100];
	
	
	
	//mi metto in attesa di un nuovo messaggio/comando
	printf("\nscrivi un messaggio da inviare al server: ");
	scanf("%s",messaggio);
    //printf("\nIl mio messaggio è %s\n", messaggio);
    //scrivo il comando sul socket
	write(socketClientDescriptor,messaggio,sizeof(messaggio));
    

    //dopo aver mandato il comando devo leggere la nuova matrice con lo spostamento
	read(socketClientDescriptor,buffer,sizeof(buffer));
    printf("\n-[]Messaggio ricevuto: \n");
       
    //stampo la matrice
    for(i=0; i<100; i++){
        if(i % 10 == 0){
            printf("\n");     
        }else{
                printf("%c ", buffer[i]);
              }
    }

    //memset(buffer, 0, sizeof(buffer));
    //close(socketClientDescriptor);
    }//fine while

	


	return 0;
}

void stampa(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			printf("%c   ", m[i][j]);

			}
			printf("\n");
		}

}
