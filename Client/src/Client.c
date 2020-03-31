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


#define PORTA 1205
#define MAX 11

int socketClientDescriptor; //variabile globale,descrittore del socket locale del client
int x;//varibile globale per la sincronizzazione dei thread
int paccoPreso= 0; //variabile booleana che indica qnd è stato premuto il pulsante p
int lista=0;  //variabile booleana lista
int fine_partita=0;

void *stampaMatriceThread(void *arg){
	x=1;
	char buffer[121];
	int punteggio=0;

	while(fine_partita==0){
			if(x==1){
				
				if(lista == 1){
					char utenti[124];//buffer usato qnd devo eggere la lista utenti
					read(socketClientDescriptor,utenti,124);
					printf("\nUtenti online: \n");
					for(int i=0; i<124; i++){
						if(utenti[i] == ',' && utenti[i] != '*'){
							printf("\n");
						}else if(utenti[i] != '*'){
							printf("%c", utenti[i]);
						}
					}
					printf(" \n");
					lista = 0;
				}
				
				int tempo[3];
				//leggo il tempo restante					
				//read(socketClientDescriptor,&tempo[0],1);
				read(socketClientDescriptor,&tempo[1],1);
				read(socketClientDescriptor,&tempo[2],1);
				printf("\nTEMPO RESTANTE: %dm:%ds\n",tempo[1],tempo[2]);
									
				//leggo il punteggio del giocatore
				read(socketClientDescriptor,&punteggio,1);
				printf("Punteggio: %d\n",punteggio);
				
				//se ha cliccato p leggo le locazioni di arrivo
				if(paccoPreso==1){
					int coordinateLocazioni[2];
					char temp;
					read(socketClientDescriptor,&coordinateLocazioni[0],1);
					read(socketClientDescriptor,&coordinateLocazioni[1],1);					
					if(coordinateLocazioni[0]==0 && coordinateLocazioni[1]== 0){
						printf("Non ce nulla da raccogliere qui!");
					}else{
						printf("Pacco preso!");
						temp = coordinateLocazioni[0]+48 ;
						temp=temp+16;
						printf("Destinazione: %c (riga)  %d(colonna )",temp,coordinateLocazioni[1]-1);
					}		
				paccoPreso=0;
				}
				
				//leggo la matrice del gioco e visualizzo il quadro
				read(socketClientDescriptor,buffer,121);
				//stampo la matrice
				for(int i=0; i<121; i++){
					if(i % 11 == 0){
						printf("\n");
						if(buffer[i] == 'o'){
							printf("* ");
						}else if(buffer[i] == 'i'){
							printf("o ");
						}else{
							printf("%c ", buffer[i]);
						}
					}else{
						if(buffer[i] == 'o'){
							printf("* ");
						}else if(buffer[i] == 'i'){
							printf("o ");
						}else{
							printf("%c ", buffer[i]);
						}
					 }
				}
			
				x=2;
			
			}//fine if(x=1)
		}
					
	
	pthread_exit(0);
}


void *leggiComandoThread(void *arg){


		while(fine_partita==0){

			if(x==2){
				char messaggio[1];
			
				//mi metto in attesa di un nuovo messaggio/comando
				do{
					printf("\nscrivi un messaggio da inviare al server: ");
					scanf("%s",messaggio);
				}while(messaggio[0] != 'w' &&
					messaggio[0] !='d' &&
					messaggio[0] !='s' &&
					messaggio[0] !='a' &&
					messaggio[0] !='p' &&
					messaggio[0] !='x' &&
					messaggio[0] !='u' &&
					messaggio[0] !='l' &&
					messaggio[0] !='t');
				
				
				if(messaggio[0] == 'p')paccoPreso=1;

				if(messaggio[0] == 'x'){
					printf("\nDISCONNESSO\n");
					fine_partita = 1;
				}

				if(messaggio[0] == 'u')lista=1;

				//scrivo il comando sul socket
				write(socketClientDescriptor,messaggio,sizeof(messaggio));
				x=1;
			}
			//dopo aver mandato il comando devo leggere la nuova matrice con lo spostamento
			//e lo faccio nel thread che stampa la matrice

		}//fine while

		pthread_exit(0);	
		
}//fine funzioneThread

//da cambiare con int main(int argc, char *argv[]) {  ???
int main(void) { 
		
		char username[30];
		char password[10];
	    char confronto[10];
	    char a;//variabile su cui inserire la pressione dei tasti
	    int proseguo =0;
	    int k;
	    
		struct sockaddr_in serverDescriptor; //struct in cui inserisco le info del server a cui voglio connettermi
					   						 //poiche il client per connettersi deve conoscere ip e porta del server
		
		//struttura alla quale mi connetto(il server cioè)
		serverDescriptor.sin_family=AF_INET;
		serverDescriptor.sin_port=htons(PORTA);
		inet_aton("192.168.1.11", &serverDescriptor.sin_addr);
		    
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
		    scanf(" %c", &a);

		    if(a >= 'a' && a <= 'z'){
		        a = a - 32;
		    }

		    if(a=='R'){
		        write(socketClientDescriptor, &a, sizeof(a));
		        printf("\nScegliere l'username: \n");
		    	scanf("%s", username);
		    	k=strlen(username);
		    	username[k] = '-';
		    	username[k+1] = '\0';
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
		    	k = strlen(username);
		    	username[k] = '-';
		    	username[k+1] = '\0';
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

		//INIZIO DEL GIOCO
		printf("\nBENVENUTO NEL GIOCO\n");
		printf("Istruzioni:\nBisogna muoversi nel labirinto, "
				"prendere il pacchetto ed uscire per "
				"completare il gioco.\n\nLegenda: \n"
				"*-Spazio libero\no-Ostacolo\nx-pacchetto\n");
		printf("\nUtilizzare i tasti W A S D della tastiera "
				"per muovere il personaggio all'interno del "
				"labirinto.\nPremere:\nU-vedere la lista "
				"utenti online\nP-prendere il pacchetto\nL-"
				"depositare il pacchetto\nT-visualizzare il t"
				"empo\nX-DISCONNETTI\n");
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
