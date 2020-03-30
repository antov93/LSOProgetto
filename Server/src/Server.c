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
#include <time.h>

#define MAX 11 //dimensione matrice
#define PORTA 1205 //porta sulla quale il server è in ascolto

typedef struct{
	int descrittore;
	char indirizzo[100];
}parametriClient;

parametriClient p[15]; //variabile globale che rappresenta tutti i client che si connettono
int numeroClient=0; //variabile globale,indice dei client connessi
char campoGioco[MAX][MAX]; //variabile globale, tutti i client lavorano sullo stesso campo 
char personaggi[]={'a','b','c','d','e','f','g','h','i','l','m','n','o','p','q','r','s','t','u','v','z'};
int personaggioCorrente=0; //indica il personaggio a cui siamo arrivati 
int ore = 0, minuti = 14, secondi = 10; //var globale tempo

void creaCampo(char m[MAX][MAX]);
void creaOstacoli(char m[MAX][MAX]);
void creaPacco(char m[MAX][MAX]);
void creaPersonaggio(char m[MAX][MAX]);
int ipos(char m[][MAX],char personaggio);
int jpos(char m[][MAX],char personaggio);
int check(char utente[]);
void registra(char utente[]);
int cercaPersonaggio(char personaggio);
void logging_log(char ip[]);
void logging_exit(char ip[]);


void *timerThread(void *arg){

	while(1){
		if(secondi < 0){
			secondi = 59;
			--minuti;
		}
		if(minuti < 0){
			printf("\nTEMPO FINITO!\n");
			pthread_exit(0);
		}
		sleep(1);
		--secondi;
	}
}

void *gestioneClientThread(void * arg){//devo passare  in arg il client connectiondescriptor,la matrice ,indirizzo
	
    		char buffer[121];
    		int clientThread=numeroClient-1; //client associato a qst thread
    		char username[30];
    		int esiste=0;//controllo dei dati utenti
    		char accesso;
    		int proseguo =0;//sistema di registrazione
  			char personaggioClient;//personaggio associato a client in questione
  			int locazioneI=0;
  			int locazioneJ=0; //coordinate della locazione di arrivo di un pacco
  			int punteggioClient=0; //numero pacchi posati
  			char ultimoCarattere = '*'; //variabile temporanea per la disconnessione
  			int tempo[3]; //buffer per inviare il tempo al client
  			char personaggiConnessi[MAX]; //vettore contenente tutti i caratteri connessi
  			int k=0;

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
  			creaPersonaggio(campoGioco);
  			creaPacco(campoGioco);
  			
  			//assegno il personaggio al client in qstione
  			//poiche personaggi è globale
  			//mentre personaggioClient è locale al thread
  			personaggioClient=personaggi[personaggioCorrente-1];
  			printf("-Il personaggio del client --> '%c'\n",personaggioClient);
  			printf("-Client numero: '%d'\n",clientThread);
  			printf("-numeroClient: '%d'\n",numeroClient);

  			logging_log(username);

  			//dopo la connessione con il client invio subito il quadro di gioco solo al client connesso
  			write(p[clientThread].descrittore,campoGioco,121);
  			/*			for(int i=0;i<numeroClient;i++){
  				write(p[i].descrittore,campoGioco,121);
  			}
  	*/
  			printf("In attesta di ricezione di un messaggio...\n");

            while(1){
              	
  				//poi dopo aspetto di leggere il porssimo comando
  				read(p[clientThread].descrittore,buffer,1/*sizeof(buffer)*/);
  				printf("-[%s]Messaggio ricevuto dal personaggio '%c' : %s\n",p[clientThread].indirizzo,personaggioClient,buffer);
  				
  				int i,j; //coordinate del del personaggio
  				int paccoI,paccoJ; //coordinate del pacco
  				int scelta;

  				scelta = buffer[0];
  				
  				//se il comando è minuscolo, allora lo rendo maiuscolo
  				if(scelta >= 'a' && scelta <= 'z'){ 
  					scelta = scelta-32; 
  				}

  				//provare ad inviare il campo solo a chi invia il comando
  				switch(scelta){ //switch di antov //assumo che nn possono esserci 2 pacchi contemporaneamente nella matrice
  					case 'A':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        printf("\nLE COORD sono i=%d e j=%d paccoI=%d e paccoJ=%d\n",i,j,paccoI,paccoJ);
  						if(campoGioco[i][j-1] == '*'){
  							if((i==paccoI && j==paccoJ) ){
  								ultimoCarattere = campoGioco[i][j-1];
  								campoGioco[i][j] = 'x';
  								campoGioco[i][j-1] = personaggioClient;
                            }else{
                            	ultimoCarattere = campoGioco[i][j-1];
                            	campoGioco[i][j] = '*';
                            	campoGioco[i][j-1] = personaggioClient;
                            }
  						}else if(campoGioco[i][j-1] == 'x'){
  							ultimoCarattere = campoGioco[i][j-1];
  							paccoI = i, paccoJ = j-1;
                            campoGioco[i][j] = '*';
  							campoGioco[i][j-1] = personaggioClient;
  						 }


  					  	write(p[clientThread].descrittore,campoGioco, 121);
  					  	printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
  					  	printf("-Client numero: '%d'\n",clientThread);

/*  						for(int i=0;i<numeroClient;i++){
 							write(p[i].descrittore,campoGioco, 121);
						}
*/
  						break;

  					case 'D':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        printf("\nLE COORD sono i=%d e j=%d paccoI=%d e paccoJ=%d\n",i,j,paccoI,paccoJ);
  						if(campoGioco[i][j+1] == '*'){
  							if((i==paccoI && j==paccoJ) ){
  								ultimoCarattere = campoGioco[i][j+1];
  								campoGioco[i][j] = 'x';
  								campoGioco[i][j+1] = personaggioClient;
                            }else{
                            	ultimoCarattere = campoGioco[i][j+1];
                            	campoGioco[i][j] = '*';
                            	campoGioco[i][j+1] = personaggioClient;
                            }

  						}else if(campoGioco[i][j+1] == 'x'){
  							ultimoCarattere = campoGioco[i][j+1];
  							paccoI = i, paccoJ = j+1;
                            campoGioco[i][j] = '*';
  							campoGioco[i][j+1] = personaggioClient;
  						 }
  						write(p[clientThread].descrittore,campoGioco, 121);
  						printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
  						printf("-Client numero: '%d'\n",clientThread);
  						/*						for(int i=0;i<numeroClient;i++){
  							write(p[i].descrittore,campoGioco, 121);
  						}
*/
  						break;

  					case 'W':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        printf("\nLE COORD sono i=%d e j=%d paccoI=%d e paccoJ=%d\n",i,j,paccoI,paccoJ);
  						if(campoGioco[i-1][j] == '*'){
  							if((i==paccoI && j==paccoJ) ){
  								ultimoCarattere = campoGioco[i-1][j];
  								campoGioco[i][j] = 'x';
  								campoGioco[i-1][j] = personaggioClient;
                            }else {
                            	ultimoCarattere = campoGioco[i-1][j];
                                campoGioco[i][j] = '*';
  								campoGioco[i-1][j] = personaggioClient;
                              }
  						}else if(campoGioco[i-1][j] == 'x'){
  							ultimoCarattere = campoGioco[i-1][j];
  							paccoI = i-1, paccoJ = j;
                            campoGioco[i][j] = '*';
  							campoGioco[i-1][j] = personaggioClient;
  						 }

  						write(p[clientThread].descrittore,campoGioco, 121);
  						printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
  						printf("-Client numero: '%d'\n",clientThread);
  						/*		for(int i=0;i<numeroClient;i++){
  							write(p[i].descrittore,campoGioco, 121);
  						}
*/
  						break;

  					case 'S':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        printf("\nLE COORD sono i=%d e j=%d paccoI=%d e paccoJ=%d\n",i,j,paccoI,paccoJ);
  						if(campoGioco[i+1][j] == '*'){
  							if((i==paccoI && j==paccoJ) ){
  								ultimoCarattere = campoGioco[i+1][j];
  								campoGioco[i][j] = 'x';
  								campoGioco[i+1][j] = personaggioClient;
                            }else {
                            	ultimoCarattere = campoGioco[i+1][j];
                            	campoGioco[i][j] = '*';
  								campoGioco[i+1][j] = personaggioClient;
                              }
  						}else if(campoGioco[i+1][j] == 'x'){
  							ultimoCarattere = campoGioco[i+1][j];
  							paccoI = i+1, paccoJ = j;
                            campoGioco[i][j] = '*';
  							campoGioco[i+1][j] = personaggioClient;
  						 }
  						printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
  						write(p[clientThread].descrittore,campoGioco, 121);
  						printf("-Client numero: '%d'\n",clientThread);
  						/*				for(int i=0;i<numeroClient;i++){
  							write(p[i].descrittore,campoGioco, 121);
  						}
*/
  						break;

  					case 'P':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        printf("paccoi==%d e paccoj==%d  i==%d e j==%d\n",paccoI, paccoJ, i, j);
                        if(i==paccoI && j == paccoJ){
                                printf("Pacco preso!\n");
                                paccoI=paccoJ=0;
                                locazioneI=rand()%(10)+1;
                                locazioneJ=rand()%(10)+1;
                                printf("Locazione di arrivo: colonna=%d riga=%d\n",locazioneJ,locazioneI);

                            }else{
                                printf("Non c'è nessun pacco da prendere!\n");
                            }

                        write(p[clientThread].descrittore,campoGioco, 121);
                        printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
                        printf("-Client numero: '%d'\n",clientThread);

                        /*                      for(int i=0;i<numeroClient;i++){
                        	write(p[i].descrittore,campoGioco, 1000);
  						}*/

  						break;

  					case 'L':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
  						if(i==locazioneI && j==locazioneJ){
  							printf("Pacco lasciato!\n");
  							punteggioClient++;
  							printf("Punteggio attuale: %d\n",punteggioClient);
  							locazioneI=locazioneJ=0;
  							if(punteggioClient<7){
  								creaPacco(campoGioco);
  							}else{
  								printf("-Il client %d ha vinto!\n",clientThread);
  							}
  						}else{
  							if(locazioneI==0 && locazioneJ==0 ){
  								printf("Non hai nessun pacco da lasciare!\n");
  							}else{
  								printf("Non puoi lasciare qui questo pacco!\n");
  							}
  						}

  						 write(p[clientThread].descrittore,campoGioco, 121);
  						 printf("-Matrice inviata al personaggio '%c'\n",personaggioClient);
  						 printf("-Client numero: '%d'\n",clientThread);


  						break;
  					case 'T':
  						/*RICHIESTA TEMPO*///////////////////////////
  						write(p[clientThread].descrittore,&ore,1);
  						write(p[clientThread].descrittore,&minuti,1);
  						write(p[clientThread].descrittore,&secondi,1);
  						break;

  					case 'U':
  						/*LISTA Client connessi*///////////////////////////////
  						for(int i=0; i<MAX; i++){
  							for(int j=0; j<MAX; j++){
  								if(cercaPersonaggio(campoGioco[i][j]) == 1 && campoGioco[i][j] != 'o'){
  									personaggiConnessi[k] = campoGioco[i][j];
  									k++;
  								}
  							}
  						}
  						personaggiConnessi[k] = '\0';
  						write(p[clientThread].descrittore,personaggiConnessi,strlen(personaggiConnessi));
  						k=0;
  						break;
  					case 'X':
  						/*DISCONNESSIONE ACCOUNT*//////////////////////////////////
  						i = ipos(campoGioco,personaggioClient);
  					  	j = jpos(campoGioco,personaggioClient);
  						campoGioco[i][j] = ultimoCarattere;
  						logging_exit(username);
  						pthread_exit(0);
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
	pthread_t th1; //thread dedicato al timer di gioco
	pthread_t th2;
    
	//genero il campo da gioco che deve essere uguale per tutti i client
    creaCampo(campoGioco);
    	
    //genero gli ostacoli uguali per ogni client 
    creaOstacoli(campoGioco);
    	
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
		
		int lunghezzaClient;
		
		lunghezzaClient=sizeof(indirizzoClient);
		
		//accetto connessioni all infinito
		clientConnectionDescriptor=accept(socketDescriptor,(struct sockaddr *)&indirizzoClient,&lunghezzaClient);
		if(clientConnectionDescriptor<0){
			perror("Errore ACCEPT del server");
			exit(0);
		}
		
		//creo il thread per il timer solo dopo la connessione del primo client


		//memorizzo i dati del client connesso nella struttura globale
		p[numeroClient].descrittore=clientConnectionDescriptor;
		strcpy(p[numeroClient].indirizzo,inet_ntoa(indirizzoClient.sin_addr));
		numeroClient++;
		
		if(numeroClient==1){
					pthread_create(&th1,NULL,timerThread, NULL);
				}

		//creo il thread per gestire il singolo client
		pthread_create(&th2,NULL,gestioneClientThread,NULL);
		
		//pthread_join(th,NULL);
		
	}//fine while delle connessioni(cioè accetto sempre connessioni)
    
	return 0;

}

//genero una matrice 11x11 dove la riga e la colonna zero rappresentano le coordinate
void creaCampo(char m[MAX][MAX]){
	
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
void creaOstacoli(char m[][MAX]){
	
	int i;
	int j;
	int cont = 0;
	
	while(cont<10){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == '*'){
			m[i][j] = 'o';
			cont++;
		}
	}
}

void creaPersonaggio(char m[][MAX]){
	
	int i;
	int j;	
	int cont = 0;
	
	
	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == '*'){
			m[i][j] = personaggi[personaggioCorrente];
			personaggioCorrente++;
			cont++;
		}
	}
}

void creaPacco(char m[][MAX]){
	int i=0,j=0;
	int cont =0;
	
	while(cont<1){
			i=rand()%(MAX-1)+0;
			j=rand()%(MAX-1)+0;
			if(m[i][j] == '*'){
				m[i][j] = 'x';
				cont++;
			}
	}
}

//ritorna la riga del personaggio dato
int ipos(char m[][MAX],char personaggio){
	
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			if(m[i][j] == personaggio){ //bisogna inserire personaggio corrente al posto della a se no funziona solo cn il primo personaggio
				return i;			
			}

		}
	}
}

//ritorna la colonna del personaggio dato
int jpos(char m[][MAX],char personaggio){
	
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			if(m[i][j] == personaggio){
				return j;			
			}

		}
	}
}

//????scorre la matrice: qnd trova v ritorna 0,qnd trova p ritorna 1 
//int preso(char m[][MAX]){
//
//	int i;
//	int j;
//
//	for(i=0; i<MAX; i++){
//		for(j=0; j<MAX; j++){
//			if(m[i][j] == 'v'){
//				return 0;			
//			}else if(m[i][j] == 'p'){
//				return 1;
//			 }
//	
//		}
//	}
//}
 
void stampa(char m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			printf(" %c ", m[i][j]);

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

//controllo se è presente tra i personaggi il personaggio dato
int cercaPersonaggio(char personaggio){

	int i=0;

	while(personaggi[i] != '\0'){
		if(personaggi[i] == personaggio){
			return 1;
		}
		i++;
	}

	return 0;
}

void logging_log(char ip[]){
    FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "Login: %s %s\n", ip,asctime(localtime(&ora)));
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}


void logging_exit(char ip[]){
    FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "Exit: %s %s\n", ip,asctime(localtime(&ora)));
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}
