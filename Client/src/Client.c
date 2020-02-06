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
#include <pthread.h>

#define PORTA 1203
#define MAX 11

int socketClientDescriptor; //variabile globale,descrittore del socket locale del client
pthread_mutex_t mutex1; //servono per sincronizzare i thread 
pthread_mutex_t mutex2;

void * stampaMatriceThread(void *arg){
		
		char buffer[1000];
		
		while(1){
			//blocco qst codice
			pthread_mutex_lock(&mutex1);
			
			//leggo la matrice del gioco e visualizzo il quadro 
			read(socketClientDescriptor,buffer,sizeof(buffer));
						
			//stampo la matrice
			for(int i=0; i<121; i++){
				if(i % 11 == 0){
					printf("\n"); 
					printf("%c ", buffer[i]);
					    
				}else{
					printf("%c ", buffer[i]);
				 }
			}
			//sblocco il codice del th2
			pthread_mutex_unlock(&mutex2);
			sleep(1);
		}
	
		pthread_exit(0);
}

void * leggiComandoThread(void *arg){
		
		while(1){
			//blocco  qst codice
			pthread_mutex_lock(&mutex2);
			char messaggio[100];
			//sleep(5);
			
			//mi metto in attesa di un nuovo messaggio/comando
			printf("\nscrivi un messaggio da inviare al server: ");
			scanf("%s",messaggio);
			
			//scrivo il comando sul socket
			write(socketClientDescriptor,messaggio,sizeof(messaggio));
			
			//dopo aver mandato il comando devo leggere la nuova matrice con lo spostamento
			//e lo faccio nel thread che stampa la matrice
			
			//sblocco il th1
			pthread_mutex_unlock(&mutex1);
			sleep(1);
		}//fine while
		
		pthread_exit(0);	
		
}//fine funzioneThread

//da cambiare con int main(int argc, char *argv[]) {  ???
int main(void) {
		
		char username[30];
		char password[10];
	    char confronto[10];
	    char a;
	    int proseguo =0;
	    
		struct sockaddr_in serverDescriptor; //struct in cui inserisco le info del server a cui voglio connettermi
											 //poiche il client per connettersi deve conoscere ip e porta del server
		
		pthread_mutex_lock(&mutex2);
		pthread_mutex_unlock(&mutex1);
		
		//struttura alla quale mi connetto(il server cioè)
		serverDescriptor.sin_family=AF_INET;
		serverDescriptor.sin_port=htons(PORTA);
		inet_aton("192.168.1.12", &serverDescriptor.sin_addr);
		    
		//creo un socket locale per il client
		socketClientDescriptor=socket(AF_INET,SOCK_STREAM,0);
		if(socketClientDescriptor<0)perror("Errore creazione client socket."),exit(0);
		    
		//connetto il socket client all indirizzo ip del server
		if( connect(socketClientDescriptor,(struct sockaddr *)&serverDescriptor,sizeof(serverDescriptor)) <0)
			perror("Errore durante la connessione al server."),exit(0);
			
		printf("!!!!!!Connesso al Server!!!!!!\n");
		
		//SISTEMA DI REGISTRAZIONE
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
		    	read(socketClientDescriptor,&proseguo,sizeof(proseguo));

                if(proseguo==0){
                    printf("\nDati errati: username o password errati!\n");
                }

		    }else{
		        printf("\nScelta errata, ripetere!\n");    
		    }

		}//fine while del sistema di registrazione	

		
		printf("\nBENVENUTO NEL GIOCO\n");
		printf("Istruzioni:\nBisogna muoversi nel labirinto, prendere il pacchetto ed uscire per completare il gioco.\n\nLegenda: \nA-personaggio\nE-Uscita\nI-Spazio libero\nO-Ostacolo\nP-Pacchetto preso\n");
		printf("\nUtilizzare i tasti W A S D della tastiera per muovere il personaggio all'interno del labirinto.\n");
		printf("Campo di gioco:\n");
		
		pthread_t th1,th2;
	  
		if( pthread_create(&th1, NULL, stampaMatriceThread, NULL) != 0 )
			printf("Failed to create thread 1\n");
		
		
		
		if( pthread_create(&th2, NULL, leggiComandoThread, NULL) != 0 )
			printf("Failed to create thread 2\n");
	
		
		
		pthread_join(th1,NULL);
		pthread_join(th2,NULL);
	
		return 0;
}

//void stampa(char buffer[]){
//
//	int i;
//	int j;
//
//	for(i=0; i<100; i++){
//	       if(buffer[i] == 'o'){
//	          if(i % 10 == 0){
//	        	  printf("\n");
//	          }
//	          printf("* ");
//	       }else{
//	        	if(i % 10 == 0){
//	        		printf("\n");
//	        	}
//	        	printf("%c ", buffer[i]); 
//	         }
//	}
//}
