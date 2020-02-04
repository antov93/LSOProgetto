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

#define PORTA 1208
#define MAX 10 

void stampa(char m[][MAX]);

int main(int argc, char *argv[]) {
	int i;
	struct sockaddr_in serverDescriptor; //struct in cui inserisco le info del server a cui voglio connettermi
								 //poiche il client per connettersi deve conoscere ip e porta del server
    char buffer[1000];
	int socketClientDescriptor; //descrittore del socket locale del client
	char username[30];
	char password[10];
    char messaggio[100];
    char confronto[10];
    char a;
    int proseguo =0;
	//struttura alla quale mi connetto(il server cioè)
	serverDescriptor.sin_family=AF_INET;
	serverDescriptor.sin_port=htons(PORTA);
	inet_aton("192.168.43.219", &serverDescriptor.sin_addr);
    
	//creo un socket locale per il client
	socketClientDescriptor=socket(AF_INET,SOCK_STREAM,0);
	if(socketClientDescriptor<0)perror("Errore creazione client socket."),exit(0);
    
	//connetto il socket client all indirizzo ip del server
	if( connect(socketClientDescriptor,(struct sockaddr *)&serverDescriptor,sizeof(serverDescriptor)) <0)
		perror("Errore durante la connessione al server."),exit(0);
	
	printf("!!!!!!Connesso al Server!!!!!!\n");

//Sistema di registrazione
 while(proseguo==0){
    printf("\nPremere 'R' per registrarsi oppure 'L' per effettuare l'accesso\n");
    scanf("%c", &a);
    
    if(a >= 'a' && a <= 'z'){
        a = a - 32;
    }

    if(a=='R'){
        write(socketClientDescriptor, &a, sizeof(a));
        printf("\nScegliere l'username: \n");
    	scanf("%s", username);

	    printf("\nScegliere una password: \n");
	    scanf("%s", password);

        printf("\nRipetere password: \n");
        scanf("%s", confronto);

        if(strcmp(password,confronto) == 0){
            char t='Y';
            write(socketClientDescriptor, &t, sizeof(t));
            strcat(username,password);
            write(socketClientDescriptor, username, sizeof(username));
        }else{
        char t='N';
	    printf("\nConfronto password errato, ripetere!\n");
        write(socketClientDescriptor, &t, sizeof(t));
	}
        
    }else if(a=='L'){

    	write(socketClientDescriptor, &a, sizeof(a));
    	printf("\nInserire username: \n");
	scanf("%s", username);

	printf("\nInserire password: \n");
	scanf("%s", password);

	strcat(username,password);

	write(socketClientDescriptor, username, sizeof(username));
    	proseguo=1;

    }else{
        printf("\nScelta errata, ripetere!\n");    
    }

}//fine while del sistema di registrazione	

    

        	
	
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
	
	
	
	//mi metto in attesa di un nuovo messaggio/comando
	printf("\nscrivi un messaggio da inviare al server: ");
	scanf("%s",messaggio);

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
