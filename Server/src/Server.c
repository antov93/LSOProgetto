/*
 ============================================================================
 Name        : Server.c
 Author      : Antonio Vanacore & Valerio Panzera
 Version     :
 Copyright   : Your copyright notice
 Description : Uso dei Socket TCP in C
 ============================================================================
 */

//compilare usando gcc -pthread nomefile.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>

#define MAX 11 //dimensione matrice
#define PORTA 1203 //porta sulla quale il server è in ascolto
//#define CODA 10//client massimi che possono accedere

typedef struct{
	int descrittore;
	char indirizzo[100];
}parametriClient;

parametriClient p[15]; //variabile globale che rappresenta tutti i client che si connettono
int numeroClient=0; //variabile globale,indice dei client connessi
char campoGioco[MAX][MAX]; //variabile globale, tutti i client lavorano sullo stesso campo 

void generazione(char m[MAX][MAX]);
void stampa(char m[MAX][MAX]);
void ostacoli(char m[MAX][MAX]);
void personaggiopacchetto(char m[MAX][MAX]);
int preso(char m[][MAX]);
int ipos(char m[][MAX]);
int jpos(char m[][MAX]);
int check(char utente[]);
void registra(char utente[]);



void *funzioneThread(void * arg){//devo passare  in arg il client connectiondescriptor,la matrice ,indirizzo
	
    		char buffer[1000];
    		int clientThread=numeroClient-1; //client associato a qst thread
    		char username[30];
    		int esiste=0;//controllo dei dati utenti
    		char accesso;
    		int proseguo =0;//sistema di registrazione
  			
    		
    		printf("**Nuova connessione dal client: %s\n",p[clientThread].indirizzo);
  			
    		
    		/////REGISTRAZIONE/////
    		while(proseguo==0){
    			    read(p[clientThread].descrittore,&accesso,sizeof(accesso));
    		        printf("\nIo so che accesso è %c\n", accesso);
    				if(accesso == 'L'){
    		                read(p[clientThread].descrittore,username,sizeof(username));
    		            	proseguo=check(username);
				write(p[clientThread].descrittore,&proseguo, sizeof(proseguo));

    		        }else if(accesso == 'R'){
    		                char t;
    		                read(p[clientThread].descrittore, &t,sizeof(t));
    		                if(t=='Y'){
    		                read(p[clientThread].descrittore,username,sizeof(username));
    		            	registra(username);
    		                }

    		          }else{
    		                printf("\nPremere 'L' o 'R'!\n");
    		            }

    			}//fine while del sistema di registrazione
    		
    		
  			//genero personaggio e pacco,ogni client è diverso
  			personaggiopacchetto(campoGioco);
  			
            //dopo la connessione con il client invio subito il quadro di gioco a tutti i client
  			for(int i=0;i<numeroClient;i++){
  				write(p[i].descrittore,campoGioco,1000);
  			}
  			
  			printf("In attesta di ricezione di un messaggio...\n");
  			
            
              
            while(1){
              	
  				//poi dopo aspetto di leggere il porssimo comando
  				read(p[clientThread].descrittore,buffer,sizeof(buffer));
  				printf("-[%s]Messaggio ricevuto: %s\n",p[clientThread].indirizzo,buffer);
  				
  				int i,j,flag,scelta;
  				scelta = buffer[0];
  				
  				//se il comando è minuscolo, allora lo rendo maiuscolo
  				if(scelta >= 'a' && scelta <= 'z'){ 
  					scelta = scelta-32; 
  				}
  				
  					switch(scelta){
  						case 'A':
  							i = ipos(campoGioco);
  							j = jpos(campoGioco);
  							flag = preso(campoGioco);
  							if(campoGioco[i][j-1] > 'b' && campoGioco[i][j-1] < 'n'){
  								if(flag == 0){
  									campoGioco[i][j] = '*';
  									campoGioco[i][j-1] = 'a';
  								}else{
  									campoGioco[i][j] = '*';
  									campoGioco[i][j-1] = 'p'; 				
  								}
  								
  							}else if(campoGioco[i][j-1] == 'v'){
  								campoGioco[i][j] = '*';
  								campoGioco[i][j-1] = 'p';					
  							}
  		
								for(int i=0;i<numeroClient;i++){
									write(p[i].descrittore,campoGioco, 1000);
								}
  						
							break;
  						
  						case 'D':
  							i = ipos(campoGioco);
  							j = jpos(campoGioco);
  							flag = preso(campoGioco);
  							if(campoGioco[i][j+1] > 'b' && campoGioco[i][j+1] < 'n'){
  								if(flag == 0){
  									campoGioco[i][j] = '*';
  									campoGioco[i][j+1] = 'a';
  								}else{
  									campoGioco[i][j] = '*';
  									campoGioco[i][j+1] = 'p'; 				
  								}
  							}else if(campoGioco[i][j+1] == 'v'){
  								campoGioco[i][j] = '*';
  								campoGioco[i][j+1] = 'p';					
  							}
  							
  							for(int i=0;i<numeroClient;i++){
								write(p[i].descrittore,campoGioco, 1000);
							}
  							
  							break;
  			
  					case 'W':
  							i = ipos(campoGioco);
  							j = jpos(campoGioco);
  							flag = preso(campoGioco);
  							if(campoGioco[i-1][j] > 'b' && campoGioco[i-1][j] < 'n'){
  								if(flag == 0){
  									campoGioco[i][j] = '*';
  									campoGioco[i-1][j] = 'a';
  								}else{
  									campoGioco[i][j] = '*';
  									campoGioco[i-1][j] = 'p'; 				
  								}
  							}else if(campoGioco[i-1][j] == 'v'){
  								campoGioco[i][j] = '*';
  								campoGioco[i-1][j] = 'p';					
  							}
  							
  							for(int i=0;i<numeroClient;i++){
  								write(p[i].descrittore,campoGioco, 1000);
  							}
  							
  						break;
  			
  					case 'S':
  							i = ipos(campoGioco);
  							j = jpos(campoGioco);
  							flag = preso(campoGioco);
  							if(campoGioco[i+1][j] > 'b' && campoGioco[i+1][j] < 'n'){
  								if(flag == 0){
  									campoGioco[i][j] = '*';
  									campoGioco[i+1][j] = 'a';
  								}else{
  									campoGioco[i][j] = '*';
  									campoGioco[i+1][j] = 'p'; 				
  								}
  							}else if(campoGioco[i+1][j] == 'v'){
  								campoGioco[i][j] = '*';
  								campoGioco[i+1][j] = 'p';					
  							}
  							
  							for(int i=0;i<numeroClient;i++){
  								write(p[i].descrittore,campoGioco, 1000);
  							}
  							
  						break;
  						
  					case 'P':
  						break;

  					case 'T':
  						break;
  						
  					default:
  						break;
  					
  					}//fine switch
  						
  				printf("Attendo altre richieste...\n");
  				
            }//fine while dei comandi(cioè accetto sempre nuovi comandi finchè nn finisco il gioco)

            
  		pthread_exit(0);
  		
}//fine funzioneThread



int main(int argc, char **argv) {

	struct sockaddr_in indirizzoServer; //serve per mettersi in ascolto su una determinata porta
	struct sockaddr_in indirizzoClient; //serve per contenere le informzioni sulla connessione che mi arrivano dal client
	int socketDescriptor; //informazioni del server
	int clientConnectionDescriptor; //info sul client arrivate tramite l ACCEPT
	int i; //indice matrice
	int j; //indice matrice
	//char m[MAX][MAX];//campo
	char scelta;
	int flag = 0; //flag controllo pacchetto
    char buffer[1000];
    
    //genero il campo da gioco che deve essere uguale per tutti i client
    generazione(campoGioco);
    	
    //genero gli ostacoli uguali per ogni client 
    ostacoli(campoGioco);
    	
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
	listen(socketDescriptor,10);
	
	while(1){
		   
		//accetto connessioni
		int lunghezzaClient;
		//int pidFiglio=0;
		lunghezzaClient=sizeof(indirizzoClient);
		clientConnectionDescriptor=accept(socketDescriptor,(struct sockaddr *)&indirizzoClient,&lunghezzaClient);
		if(clientConnectionDescriptor<0){
			perror("Errore ACCEPT del server");
			exit(0);
		}
		
		
		
		//memorizzo i dati del client connesso nella struttura globale
		p[numeroClient].descrittore=clientConnectionDescriptor;
		strcpy(p[numeroClient].indirizzo,inet_ntoa(indirizzoClient.sin_addr));
		numeroClient++;
		
		pthread_t th;
		//int i=0;
		
		pthread_create(&th,NULL,funzioneThread,NULL);
		
		//pthread_join(th,NULL);
		
    
	}//fine while delle connessioni(cioè accetto sempre connessioni)
    
	return 0;

}

//genero una matrice 11x11 dove la riga e la colonna zero rappresentano le coordinate
void generazione(char m[MAX][MAX]){
	int i,j;
	//tutte le celle sono *
	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			m[i][j] = '*';
		}
	}
	

	m[0][0]=' ';
	m[1][0]='A';
	m[2][0]='B';
	m[3][0]='C';
	m[4][0]='D';
	m[5][0]='E';
	m[6][0]='F';
	m[7][0]='G';
	m[8][0]='H';
	m[9][0]='I';
	m[10][0]='L';
	m[0][1]='0';
	m[0][2]='1';
	m[0][3]='2';
	m[0][4]='3';
	m[0][5]='4';
	m[0][6]='5';
	m[0][7]='6';
	m[0][8]='7';
	m[0][9]='8';
	m[0][10]='9';

	
				

}

//genero 10 ostacoli casuali
void ostacoli(char m[][MAX]){
	int i;
	int j;
	int cont = 0;
	int casuale;
	
	
	casuale=rand()%20+11;
	while(cont<10){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == '*'){
			m[i][j] = 'o';
			cont++;
		}
	}
}

void personaggiopacchetto(char m[][MAX]){
	
	int i;
	int j;	
	int cont = 0;

	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == '*'){
			m[i][j] = 'a';
			cont++;
		}
	}

	cont = 0;
	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == '*'){
			m[i][j] = 'v';
			cont++;
		}
	}
	
}

//ritorna la riga del personaggio
int ipos(char m[][MAX]){
	
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			if(m[i][j] == 'a' || m[i][j] == 'p'){
				return i;			
			}

		}
	}
}

//ritorna la colonna del personaggio
int jpos(char m[][MAX]){
	
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			if(m[i][j] == 'a' || m[i][j] == 'p'){
				return j;			
			}

		}
	}
}

//????scorre la matrice: qnd trova v ritorna 0,qnd trova p ritorna 1 
int preso(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			if(m[i][j] == 'v'){
				return 0;			
			}else if(m[i][j] == 'p'){
				return 1;
			 }
	
		}
	}
}
 
void stampa(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			printf("%c ", m[i][j]);

			}
			printf("\n");
		}

}

int check(char utente[]){
	
	int i=0,j=0;
	FILE *fp;
	char lettura[20][20];
	int dim;
	
	fp=fopen("utenti.txt","r"); //apro il file in lettura
	if(fp){ 
		while(!feof(fp)){
			
			fscanf(fp, "%s", lettura[i]);
			i++;
				
		}//fine while della lettura file
		
	}else{
		printf("Errore apertura file!");
	 }//fine controllo esistenza file
	
	fclose(fp);
	
	
	int res = 0;
	dim = i;
	for (i = 0; i < dim; i++) //controllo riga per riga se i dati sono presenti nel file
		if (strcmp(utente, lettura[i]) == 0){
            res = 1;
            return res;
        }
			
	  return res;
	
}

void registra(char utente[]){
    FILE *fp;
    fp=fopen("utenti.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "%s\n", utente);
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}
