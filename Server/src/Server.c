/*
 ============================================================================
 Name        : Server.c
 Author      : Antonio Vanacore
 Version     :
 Copyright   : Your copyright notice
 Description : Uso dei Socket TCP in C
 ============================================================================
 */
//comestai
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
#define MAX 10 //dimensione matrice
#define PORTA 1205 //porta sulla quale il server è in ascolto

void generazione(char m[][MAX]);
void stampa(char m[][MAX]);
void ostacoli(char m[][MAX]);
void personaggiopacchetto(char m[][MAX]);
int preso(char m[][MAX]);
int ipos(char m[][MAX]);
int jpos(char m[][MAX]);

int main(int argc, char **argv) {

	struct sockaddr_in indirizzoServer; //serve per mettersi in ascolto su una determinata porta
	struct sockaddr_in indirizzoClient; //serve per contenere le informzioni sulla connessione che mi arrivano dal client
	int socketDescriptor; //informazioni del server
	int clientConnectionDescriptor; //info sul client arrivate tramite l ACCEPT
	int i; //indice matrice
	int j; //indice matrice
	char m[MAX][MAX];//campo
	char scelta;
	int flag = 0; //flag controllo pacchetto
    int k=0; //indice buffer

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
    

    //inizializzazione campo
    generazione(m);
	ostacoli(m);
	personaggiopacchetto(m);

    printf("Bisogna muoversi nel labirinto, prendere il pacchetto ed uscire per completare il gioco.\n\nLegenda: \nA-personaggio\nE-Uscita\nI-Spazio libero\nO-Ostacolo\nP-Pacchetto preso\n");
    char buffer[1000];
	while(1 && m[MAX-1][MAX-1] != 'P'){
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
            printf("\nUtilizzare WASD per muovere il personaggio all'interno del labirinto.\n");
            stampa(m);

			
			read(clientConnectionDescriptor,buffer,sizeof(buffer));

            printf("-[%s]Messaggio ricevuto: %s\n",inet_ntoa(indirizzoClient.sin_addr),buffer);
            
            scelta = buffer[k];
            printf("il buffer è %c\n", buffer[k]);
            

		switch(scelta){
			case 'A':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j-1] != 'O' && m[i][j-1] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i][j-1] = 'A';
					}else{
						m[i][j] = 'I';
						m[i][j-1] = 'P'; 				
					}
					
				}else if(m[i][j-1] == 'V'){
					m[i][j] = 'I';
					m[i][j-1] = 'P';					
				}
				stampa(m);
            break;
            
            case 'D':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j+1] != 'O' && m[i][j+1] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i][j+1] = 'A';
					}else{
						m[i][j] = 'I';
						m[i][j+1] = 'P'; 				
					}
				}else if(m[i][j+1] == 'V'){
					m[i][j] = 'I';
					m[i][j+1] = 'P';					
				}
				stampa(m);
            break;

	    case 'W':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i-1][j] != 'O' && m[i-1][j] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i-1][j] = 'A';
					}else{
						m[i][j] = 'I';
						m[i-1][j] = 'P'; 				
					}
				}else if(m[i-1][j] == 'V'){
					m[i][j] = 'I';
					m[i-1][j] = 'P';					
				}
				stampa(m);
            break;

	    case 'S':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i+1][j] != 'O' && m[i+1][j] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i+1][j] = 'A';
					}else{
						m[i][j] = 'I';
						m[i+1][j] = 'P'; 				
					}
				}else if(m[i+1][j] == 'V'){
					m[i][j] = 'I';
					m[i+1][j] = 'P';					
				}
				stampa(m);
            break;
				case 'a':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j-1] != 'O' && m[i][j-1] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i][j-1] = 'A';
					}else{
						m[i][j] = 'I';
						m[i][j-1] = 'P'; 				
					}
					
				}else if(m[i][j-1] == 'V'){
					m[i][j] = 'I';
					m[i][j-1] = 'P';					
				}
				stampa(m);
            break;
            
            case 'd':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j+1] != 'O' && m[i][j+1] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i][j+1] = 'A';
					}else{
						m[i][j] = 'I';
						m[i][j+1] = 'P'; 				
					}
				}else if(m[i][j+1] == 'V'){
					m[i][j] = 'I';
					m[i][j+1] = 'P';					
				}
				stampa(m);
            break;

	    case 'w':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i-1][j] != 'O' && m[i-1][j] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i-1][j] = 'A';
					}else{
						m[i][j] = 'I';
						m[i-1][j] = 'P'; 				
					}
				}else if(m[i-1][j] == 'V'){
					m[i][j] = 'I';
					m[i-1][j] = 'P';					
				}
				stampa(m);
            break;

	    case 's':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i+1][j] != 'O' && m[i+1][j] != 'V'){
					if(flag == 0){
						m[i][j] = 'I';
						m[i+1][j] = 'A';
					}else{
						m[i][j] = 'I';
						m[i+1][j] = 'P'; 				
					}
				}else if(m[i+1][j] == 'V'){
					m[i][j] = 'I';
					m[i+1][j] = 'P';					
				}
				stampa(m);
            break;
			
				}//fine switch

	    			
			printf("Attendo altre richieste...\n");

            write(clientConnectionDescriptor, m, 1000); //#######

            memset(buffer, 0, sizeof(buffer));

			exit(0);
		 }

		
    k++;
    close(clientConnectionDescriptor);
	}//fine while
    
	return 0;

}

void generazione(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			m[i][j] = 'I';

			}
		}

}

void ostacoli(char m[][MAX]){
	int i;
	int j;
	int cont = 0;
	int casuale;

	casuale=rand()%20+11;
	while(cont<casuale){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 'I'){
			m[i][j] = 'O';
			cont++;
		}
	}

	m[MAX-1][MAX-1] = 'E';
}

void personaggiopacchetto(char m[][MAX]){
	
	int i;
	int j;	
	int cont = 0;

	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 'I'){
			m[i][j] = 'A';
			cont++;
		}
	}

	cont = 0;
	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 'I'){
			m[i][j] = 'V';
			cont++;
		}
	}
	
	
}

int ipos(char m[][MAX]){
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 'A' || m[i][j] == 'P'){
				return i;			
			}

}
}
}


int jpos(char m[][MAX]){
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 'A' || m[i][j] == 'P'){
				return j;			
			}

}
}
}

int preso(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 'V'){
				return 0;			
			}else if(m[i][j] == 'P'){
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
			printf("%c   ", m[i][j]);

			}
			printf("\n");
		}

}
