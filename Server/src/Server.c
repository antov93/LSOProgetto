/*
 ============================================================================
 Name        : Server.c
 Author      : Antonio Vanacore & Valerio Panzera
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
#define PORTA 1202
#define MAX 10 //dimensione matrice

void generazione(char m[][MAX]);
void stampa(char m[][MAX]);
void ostacoli(char m[][MAX]);
void personaggiopacchetto(char m[][MAX]);
int preso(char m[][MAX]);
int ipos(char m[][MAX]);
int jpos(char m[][MAX]);
int check(char utente[]);
void registra(char utente[]);

int main(int argc, char **argv){

	struct sockaddr_in indirizzoServer; //serve per mettersi in ascolto su una determinata porta
	struct sockaddr_in indirizzoClient; //serve per contenere le informzioni sulla connessione che mi arrivano dal client
	int socketDescriptor; //informazioni del server
	int clientConnectionDescriptor; //info sul client arrivate tramite l ACCEPT
	int i; //indice matrice
	int j; //indice matrice
	char m[MAX][MAX];//campo
	char scelta;
	int flag = 0; //flag controllo pacchetto
    char username[30];
   	char buffer[1000];
	int esiste=0;//controllo dei dati utenti
    char accesso;
    int proseguo =0;//sistema di registrazione
    
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
			 
			//inizializzazione campo
			    generazione(m);
				ostacoli(m);
				personaggiopacchetto(m);

			close(socketDescriptor); //lavora solo con clientConnectionDescriptor

			printf("**Nuova connessione dal client: %s\n",inet_ntoa(indirizzoClient.sin_addr));


	while(proseguo==0){
	    read(clientConnectionDescriptor,&accesso,sizeof(accesso));
            printf("\nIo so che accesso è %c\n", accesso);
		if(accesso == 'L'){
                read(clientConnectionDescriptor,username,sizeof(username));
            	esiste=check(username);
		        proseguo=1;
                
            }else if(accesso == 'R'){
                char t;
                read(clientConnectionDescriptor, &t,sizeof(t));
                if(t=='Y'){
                read(clientConnectionDescriptor,username,sizeof(username));
            	registra(username);
                }

            }else{
                printf("\nScelta errata!\n");
            }

	}//fine while del sistema di registrazione


           //dopo la connessione con il client invio subito il quadro di gioco
	   write(clientConnectionDescriptor, m, 1000);

	    if(esiste==1){//se il controllo dati è andato bene, il gioco diventa disponibile
	
            int partitaFinita=0;
            
            while(partitaFinita==0){
            	
				//poi dopo aspetto di leggere il porssimo comando
				read(clientConnectionDescriptor,buffer,sizeof(buffer));
				printf("-[%s]Messaggio ricevuto: %s\n",inet_ntoa(indirizzoClient.sin_addr),buffer);
				
				scelta = buffer[0];

                if(scelta >= 'a' && scelta <= 'z'){ //se il comando è minuscolo, allora lo rendo maiuscolo
                    scelta = scelta-32; 
                 }

					switch(scelta){
						case 'A':
							i = ipos(m);
							j = jpos(m);
							flag = preso(m);
							if((m[i][j-1] > 'b' && m[i][j-1] < 'n')){
								if(flag == 0){
									m[i][j] = 'i';
									m[i][j-1] = 'a';
								}else{
									m[i][j] = 'i';
									m[i][j-1] = 'p'; 				
								}
								
							}else if(m[i][j-1] == 'v'){
								m[i][j] = 'i';
								m[i][j-1] = 'p';					
							}
                            if(flag==0){
							    write(clientConnectionDescriptor, m, 1000);
                            }

						break;
						
						case 'D':
							i = ipos(m);
							j = jpos(m);
							flag = preso(m);
							if(m[i][j+1] > 'b' && m[i][j+1] < 'n'){
								if(flag == 0){
									m[i][j] = 'i';
									m[i][j+1] = 'a';
								}else{
									m[i][j] = 'i';
									m[i][j+1] = 'p'; 				
								}
							}else if(m[i][j+1] == 'v'){
								m[i][j] = 'i';
								m[i][j+1] = 'p';					
							}

							    write(clientConnectionDescriptor, m, 1000);
                            
						break;
			
					case 'W':
							i = ipos(m);
							j = jpos(m);
							flag = preso(m);
							if(m[i-1][j] > 'b' && m[i-1][j] < 'n'){
								if(flag == 0){
									m[i][j] = 'i';
									m[i-1][j] = 'a';
								}else{
									m[i][j] = 'i';
									m[i-1][j] = 'p'; 				
								}
							}else if(m[i-1][j] == 'v'){
								m[i][j] = 'i';
								m[i-1][j] = 'p';					
							}

							    write(clientConnectionDescriptor, m, 1000);
                            
						break;
			
					case 'S':
							i = ipos(m);
							j = jpos(m);
							flag = preso(m);
							if(m[i+1][j] > 'b' && m[i+1][j] < 'n'){
								if(flag == 0){
									m[i][j] = 'i';
									m[i+1][j] = 'a';
								}else{
									m[i][j] = 'i';
									m[i+1][j] = 'p'; 				
								}
							}else if(m[i+1][j] == 'v'){
								m[i][j] = 'i';
								m[i+1][j] = 'p';					
							}
							    write(clientConnectionDescriptor, m, 1000);
                            
						break;

                    case 'P':
							

						break;

                    case 'T':
							

						break;
						
							}//fine switch
	/*			
	 * |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	 * |||||||||||PARTE DA AGGIUNGERE|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\\
				
				//Partita terminata
				if(pacchetto preso && posizione personaggio==m[Max-1][MAX-1]){
					partitaFinita=1;
				}
					
				//Uscita dal gioco
				if(partitaFinita==1){
					printf("Bravo hai vinto una banana!");
					exit(0);
				}
		||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\
		||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||		
	*/
						
				printf("Attendo altre richieste...\n");

            }//fine while dei comandi(cioè accetto sempre nuovi comandi finchè nn finisco il gioco)

	    }else{
		printf("\nErrore durante l'accesso: username e/o password errati!");
	    }//fine controllo del sistema di registrazione

		 
		}//fine else if processo figlio
            
		
		close(clientConnectionDescriptor);
    
	}//fine while delle connessioni(cioè accetto sempre connessioni)
    
	return 0;

}

void generazione(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			m[i][j] = 'i';

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
	
		if(m[i][j] == 'i'){
			m[i][j] = 'o';
			cont++;
		}
	}

    char coord[12]="ABCDEFGHI";
    char coord2[12]="012345678";

    for(i=0, j=0; i<10; i++){
     m[i][j]=coord[i];
    }

    for(j=0, i=0; j<10; j++){
     m[i][j]=coord2[j];
    }

    for(i=0, j=9; i<10; i++){
     m[i][j]='o';
    }

     for(i=9, j=0; j<10; j++){
     m[i][j]='o';
    }

    m[9][0] = 'L';
    m[0][9] = '9';

}

void personaggiopacchetto(char m[][MAX]){
	
	int i;
	int j;	
	int cont = 0;

	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 'i'){
			m[i][j] = 'a';
			cont++;
		}
	}

	cont = 0;    
	while(cont<5){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 'i'){
			m[i][j] = 'v';
			cont++;
		}
	}
}

int ipos(char m[][MAX]){
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 'a' || m[i][j] == 'p'){
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
			if(m[i][j] == 'a' || m[i][j] == 'p'){
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
			printf("%c   ", m[i][j]);

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

