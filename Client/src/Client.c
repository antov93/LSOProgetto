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
int sincro;
int paccoPreso= 0; //variabile che indica qnd è stato premuto il tasto p
int paccoLasciato= 0; ////variabile che indica qnd è stato premuto il tasto l
int lista=0;  //variabile booleana lista
int disconnesso=0;
int partiteVinte=0;

void *stampaMatriceThread(void *arg){
	char buffer[121];
	int punteggio=0;
	int vittoria=0;
	sincro=1;

	while(disconnesso==0){
			if(sincro==1){
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
				
				//leggo il tempo restante					
				int tempo[3];
				read(socketClientDescriptor,&tempo[1],1);
				read(socketClientDescriptor,&tempo[2],1);
				printf("\nTEMPO RESTANTE: %dm:%ds\n",tempo[1],tempo[2]);
				printf("Partite vinte: %d\n",partiteVinte);

				//leggo il punteggio del giocatore
				read(socketClientDescriptor,&punteggio,1);
				printf("Punteggio: %d\n",punteggio);
				
				read(socketClientDescriptor,&vittoria,1);

				if(vittoria==1){
					printf("Qualcuno ha raccolto tutti i pacchi e ha vinto!!\n");
					printf("Start nuova partita fai la tua mossa...");
					paccoPreso=0;
					paccoLasciato=0;
				}

				if(vittoria==2){
					printf("!!!!!Hai raccolto tutti i pacchi!!!!!\n");
					printf("!!!HAI VINTO!!!\n");
					printf("Start nuova partita fai la tua mossa...");
					partiteVinte++;
					paccoPreso=0;
					paccoLasciato=0;

				}

				if(vittoria==3){
					printf("!!!TEMPO SCADUTO!!!\n");
					printf("Non hai vinto, ritenta!\n");
					printf("Start nuova partita fai la tua mossa...");
					paccoPreso=0;
					paccoLasciato=0;
				}

				if(vittoria==4){
					printf("!!!TEMPO SCADUTO!!!\n");
					printf("Hai raccolto piu pacchi di tutti!\n");
					printf("!!!HAI VINTO!!!\n");
					printf("Start nuova partita fai la tua mossa...");
					partiteVinte++;
					paccoPreso=0;
					paccoLasciato=0;

				}

				//se ha cliccato p leggo le locazioni di arrivo
				if(paccoPreso==1 ){
					int coordinateLocazioni[2];
					char temp;
					read(socketClientDescriptor,&coordinateLocazioni[0],1);
					read(socketClientDescriptor,&coordinateLocazioni[1],1);					
					if(coordinateLocazioni[0]==0 && coordinateLocazioni[1]== 0){
						printf("Non ce nulla da raccogliere qui!");
					}else{
						printf("Pacco preso!");
						temp = coordinateLocazioni[0] ; //sto usando la i
						temp = temp+48 ; //sto usando la i
						temp=temp+16;
						if(temp==74){
							temp=76;
						}
						printf("Destinazione-> %c%d",temp,coordinateLocazioni[1]-1);
					}		
					paccoPreso=0;
				}

				if(paccoLasciato==1 ){
					int preso=0;
					char temp;

					read(socketClientDescriptor,&preso,1);

					if(preso==1 ){
						printf("Pacco lasciato!");
					}

					if(preso==2){
						printf("Non hai nessun pacco da lasciare!");
					}

					if(preso==3){
						printf("Non puoi lasciare qui questo pacco!");
					}

					paccoLasciato=0;
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
				sincro=2;
			}//fine if(x=1)
		}
	pthread_exit(0);
}

void *leggiComandoThread(void *arg){
		while(disconnesso==0){
			if(sincro==2){
				char messaggio[1];
			
				//mi metto in attesa di un nuovo messaggio/comando
				do{
					printf("\ninvia un comando: ");
					scanf("%s",messaggio);
				}while(messaggio[0] != 'w' &&
					messaggio[0] !='d' &&
					messaggio[0] !='s' &&
					messaggio[0] !='a' &&
					messaggio[0] !='p' &&
					messaggio[0] !='x' &&
					messaggio[0] !='u' &&
					messaggio[0] !='l' );
				
				if(messaggio[0] == 'p')paccoPreso=1;

				if(messaggio[0] == 'l')paccoLasciato=1;

				if(messaggio[0] == 'u')lista=1;

				//scrivo il comando sul socket
				write(socketClientDescriptor,messaggio,sizeof(messaggio));

				if(messaggio[0] == 'x'){
					printf("\n***DISCONNESSO\n");
					disconnesso=1;
					sincro=0;
				}else{
					sincro=1;
				}

			}//fine if
		}//fine while
	pthread_exit(0);
}//fine funzioneThread

int main(int argc, char *argv[]) {
		char username[30];
		char password[10];
	    char confronto[10];
	    char a;//variabile su cui inserire la pressione dei tasti
	    int proseguo =0;
	    int k;
	    struct sockaddr_in serverDescriptor; //struct in cui inserisco le info del server a cui voglio connettermi
		struct in_addr conversione;

		//poiche il client per connettersi deve conoscere ip e porta del server
		
		//struttura alla quale mi connetto(il server cioè)
		serverDescriptor.sin_family=AF_INET;
		serverDescriptor.sin_port=htons(atoi(argv[2])); //argv[2] è il secondo parametro passato
		serverDescriptor.sin_addr.s_addr= inet_addr(argv[1]);

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
				"depositare il pacchetto\nX-DISCONNETTI\n");
		printf("Campo di gioco:\n");
		
		pthread_t th1,th2;
	  
		if( pthread_create(&th1, NULL, stampaMatriceThread, NULL) != 0 )
			printf("Failed to create thread 1\n");

		if( pthread_create(&th2, NULL, leggiComandoThread, NULL) != 0 )
			printf("Failed to create thread 2\n");

		pthread_join(th2,NULL);
		pthread_join(th1,NULL);
		printf("Fine Programma.\n");

		return 0;
}
