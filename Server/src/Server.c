/*
 ============================================================================
 Name        : Server.c
 Author      : Antonio Vanacore & Valerio Panzera
 Version     :
 Copyright   : Your copyright notice
 Description : Uso dei Socket TCP in C
 ============================================================================
 */

//compilare usando gcc nomefile.c -lpthread
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

parametriClient p[100]; //variabile globale che rappresenta tutti i client che si connettono
int numeroClient=0; //variabile globale,indice dei client connessi
int clientConnessi=0; //var globale che indica attualmente il numero di client connessi
char campoGioco[MAX][MAX]; //variabile globale, tutti i client lavorano sullo stesso campo 
char personaggi[]={'a','b','c','d','e','f','g','h','l','m','n','p','q','r','s','t','u','v','z'};
int personaggioCorrente=0; //indica il personaggio a cui siamo arrivati 
int ore = 0, minuti = 1, secondi = 45; //var globale tempo
char username_globali[124]; //ci inserisco gli user dei giocatori online
int q = 0; //indice variabile username_globali
int vittoriaVettore[100];
int punteggioClient[100];//ci memorizzo i punteggi dei vari client in gioco

void creaCampo(char m[MAX][MAX]);
void creaOstacoli(char m[MAX][MAX]);
void creaPacco(char m[MAX][MAX]);
char creaPersonaggio(char m[MAX][MAX]);
int ipos(char m[][MAX],char personaggio);
int jpos(char m[][MAX],char personaggio);
int check(char utente[]);
void registra(char utente[]);
int cercaPersonaggio(char personaggio);
void logging_log(char ip[]);
void logging_exit(char ip[]);
char *salva_username(char username[]);
void cancella_online(char username[]);
void logging_posato(char username[], int i, int j, int punteggio);
void logging_preso(char username[], int i, int j, int punteggio);
void resetPersonaggiConnessi();
void inviaVittoria(int id);
void azzeraPunteggi();
void inviaVittoriaTempo();

void *timerThread(void *arg){
	while(1){
		if(secondi < 0){
			secondi = 59;
			--minuti;
		}
		if(minuti < 0){
			inviaVittoriaTempo();
			minuti=1;
			secondi=45;
			resetPersonaggiConnessi();
			azzeraPunteggi();
  		}
		sleep(1);
		--secondi;
	}
}

void *gestioneClientThread(void * arg){
    		char buffer[121];
    		int idClient=numeroClient-1; //client associato a qst thread
    		char username[30];
    		char nome[20];
    		int esiste=0;//controllo dei dati utenti
    		char accesso;
    		int proseguo =0;//sistema di registrazione
  			char personaggioClient;//personaggio associato a client in questione
  			int locazioneI=0;
  			int locazioneJ=0; //coordinate della locazione di arrivo di un pacco
  			char ultimoCarattere = '*'; //variabile temporanea per la disconnessione
  			int disconnesso=0;
  			char personaggiConnessi[MAX]; //vettore contenente tutti i caratteri connessi
  			int k=0;
  			punteggioClient[idClient]=0; //numero pacchi posati
  			char *user;

  			printf("**Richiesta connessione dal client %s\n",p[idClient].indirizzo);
    		
    		/////REGISTRAZIONE/////
    		while(proseguo==0){
    			read(p[idClient].descrittore,&accesso,sizeof(accesso));
    		    printf("-[%s]Opzione di accesso selezionata: '%c'\n",p[idClient].indirizzo,accesso);
    		    if(accesso == 'L'){
    		    	read(p[idClient].descrittore,username,sizeof(username));
    		        proseguo=check(username);
    		        write(p[idClient].descrittore,&proseguo, sizeof(proseguo));
    		    }else if(accesso == 'R'){
    		    	char t;
    		        read(p[idClient].descrittore, &t,sizeof(t));
    		        if(t=='Y'){
    		        	read(p[idClient].descrittore,username,sizeof(username));
    		            registra(username);
    		        }
    		     }else{
    		    	  printf("\nPremere 'L' o 'R'!\n");
    		      }
    		}//fine while del sistema di registrazione
    		
    		printf("***Connessione riuscita con %s\n",p[idClient].indirizzo);

    		//inserisco il nome del personaggio nella variabile globale
    		user=salva_username(username);
    		for(int q=0;q<20;q++){
    			nome[q]=user[q];
    		}

    		//genero personaggio e pacco,ogni client è diverso e salvo il carattere del client
  			personaggioClient=creaPersonaggio(campoGioco);
  			creaPacco(campoGioco);

  			printf("-[%s]ID client: %d\n",p[idClient].indirizzo,idClient);
  			printf("-[%s]User: %s\n",p[idClient].indirizzo,nome);
  			printf("-[%s]Personaggio assegnato al client: %c\n",p[idClient].indirizzo,personaggioClient);
  			printf("*Client connessi: %d\n",clientConnessi);

  			logging_log(nome);
  			vittoriaVettore[idClient]=0; //lo pongo a zero altrimenti se entro
  										//subito dopo che qlkn ha vinto mi
  									   //appare il quadro di sconfitta

  			//dopo la connessione con il client invio subito il 
  			//quadro di gioco il tempo  e il punteggio attuale solo al client connesso
  			write(p[idClient].descrittore,&minuti,1);
  			write(p[idClient].descrittore,&secondi,1);
  			write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  			write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  			write(p[idClient].descrittore,campoGioco,121);

  			printf("Attendo richieste...\n");

            while(disconnesso==0){
  				//poi dopo aspetto di leggere il porssimo comando
  				read(p[idClient].descrittore,buffer,1);
  				printf("\n-[%s][ID: %d]Messaggio ricevuto: %s \n",p[idClient].indirizzo,idClient,buffer);

  				int i,j; //coordinate del del personaggio
  				int paccoI,paccoJ; //coordinate del pacco
  				int scelta;

  				scelta = buffer[0];
  				
  				//se il comando è minuscolo, allora lo rendo maiuscolo
  				if(scelta >= 'a' && scelta <= 'z'){ 
  					scelta = scelta-32; 
  				}

  				switch(scelta){
  					case 'A':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
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
  						 }else if(campoGioco[i][j-1] == 'o'){
 							campoGioco[i][j-1] = 'i';	
 						 }
  						
  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						write(p[idClient].descrittore,campoGioco, 121);
  					  	printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;

  					case 'D':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
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
  						 }else if(campoGioco[i][j+1] == 'o'){
 							campoGioco[i][j+1] = 'i';
 						 }
  						
  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						write(p[idClient].descrittore,campoGioco, 121);
  						printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;

  					case 'W':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
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
  						 }else if(campoGioco[i-1][j] == 'o'){
 							campoGioco[i-1][j] = 'i';
 						 }
  						
  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						write(p[idClient].descrittore,campoGioco, 121);
  						printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;

  					case 'S':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
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
  						 }else if(campoGioco[i+1][j] == 'o'){
 							campoGioco[i+1][j] = 'i';
 						 }

  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						write(p[idClient].descrittore,campoGioco, 121);

  						printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;

  					case 'P':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
                        if(i==paccoI && j == paccoJ){
                        	printf("-[%s]Pacco preso!\n",p[idClient].indirizzo);
                        	logging_preso(nome, i, j, punteggioClient[idClient]);
                            paccoI=paccoJ=0;
                            do{
                            	locazioneI=rand()%(10)+1;
                                locazioneJ=rand()%(10)+1;
                            }while(campoGioco[locazioneI][locazioneJ]== 'o' );
                                
                        }else{
                        	printf("-[%s]Nessun pacco da prendere\n",p[idClient].indirizzo);
                        }
                        
                        write(p[idClient].descrittore,&minuti,1);
                        write(p[idClient].descrittore,&secondi,1);
                        write(p[idClient].descrittore,&punteggioClient[idClient], 1);
					    write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
                        if( vittoriaVettore[idClient] != 1 )write(p[idClient].descrittore,&locazioneI, 1);
                        if( vittoriaVettore[idClient] != 1 )write(p[idClient].descrittore,&locazioneJ, 1);
                        write(p[idClient].descrittore,campoGioco, 121);

                        printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;

  					case 'L':
  						i = ipos(campoGioco,personaggioClient);
  						j = jpos(campoGioco,personaggioClient);
  						int preso=0;
  						if(i==locazioneI && j==locazioneJ){
  							printf("-[%s]Pacco lasciato!\n",p[idClient].indirizzo);
  							preso=1;
  							punteggioClient[idClient]++;
  							logging_posato(nome, i, j, punteggioClient[idClient]);
  							printf("-[%s]Punteggio attuale: %d\n",p[idClient].indirizzo,punteggioClient[idClient]);
  							locazioneI=locazioneJ=0;
  							if(punteggioClient[idClient]<3){
  								creaPacco(campoGioco);
  							}else{
  								azzeraPunteggi();
  								printf("-[%s]Il client %d ha vinto la partita!\n",p[idClient].indirizzo,idClient);
  								inviaVittoria(idClient);
  								resetPersonaggiConnessi();
								minuti=5;
								secondi=5;
  							}
  						}else{
  							if(locazioneI==0 && locazioneJ==0 ){
  								preso=2;
  								printf("-[%s]Nessun pacco da lasciare.\n",p[idClient].indirizzo);
  							}else{
  								preso=3;
  								printf("-[%s]Locazione sbagliata\n",p[idClient].indirizzo);
  							}
  						}

  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient],1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						if( vittoriaVettore[idClient] != 1 && vittoriaVettore[idClient] !=2 )write(p[idClient].descrittore,&preso, 1);
  						write(p[idClient].descrittore,campoGioco, 121);

  						printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						preso=0;

  						break;

  					case 'U':
  						/*RICHIESTA LISTA Client connessi*/
  						write(p[idClient].descrittore,username_globali,124);
  						write(p[idClient].descrittore,&minuti,1);
  						write(p[idClient].descrittore,&secondi,1);
  						write(p[idClient].descrittore,&punteggioClient[idClient], 1);
  						write(p[idClient].descrittore,&vittoriaVettore[idClient],1);
  						write(p[idClient].descrittore,campoGioco, 121);

  						printf("-[%s]Matrice inviata\n",p[idClient].indirizzo);

  						break;
  					
  					case 'X':
  						/*RICHIESTA DISCONNESSIONE ACCOUNT*/
  						i = ipos(campoGioco,personaggioClient);
  					  	j = jpos(campoGioco,personaggioClient);
  					    printf("**[%s]Richiesta disconnessione\n",p[idClient].indirizzo);
  					    campoGioco[i][j] = ultimoCarattere;
  					    campoGioco[ipos(campoGioco,'x')][jpos(campoGioco,'x')]='*';
  					    logging_exit(nome);
  						cancella_online(username);
  						clientConnessi--;
  						punteggioClient[idClient]=0;
  						disconnesso=1;
  				        printf("***[%s]Disconnesso.\n",p[idClient].indirizzo);

  						break;

  					default:

  						break;

  					}//fine switch
  					vittoriaVettore[idClient]=0;
  					printf("Attendo altre richieste...\n");
            }//fine while dei comandi(cioè accetto sempre nuovi comandi finchè nn finisco il gioco)
       //se è arrivata la richiesta di disconnessione chiudo la comunicazione
       close(p[idClient].descrittore);
}//fine funzioneThread

int main(int argc, char **argv) {
	struct sockaddr_in indirizzoServer; //serve per mettersi in ascolto su una determinata porta
	struct sockaddr_in indirizzoClient; //serve per contenere le informzioni sulla connessione che mi arrivano dal client
	int socketDescriptor; //informazioni del server
	int clientConnectionDescriptor; //info sul client arrivate tramite l ACCEPT
	pthread_t th1; //thread dedicato al timer di gioco
	pthread_t th2; //thread dedicato al gestione di ogni client
    
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
		
		//memorizzo i dati del client connesso nella struttura globale
		p[numeroClient].descrittore=clientConnectionDescriptor;
		strcpy(p[numeroClient].indirizzo,inet_ntoa(indirizzoClient.sin_addr));
		numeroClient++;
		clientConnessi++;
		
		//creo il thread per il timer solo dopo la connessione del primo client
		if(numeroClient==1){
					pthread_create(&th1,NULL,timerThread, NULL);
		}

		//creo il thread per gestire il singolo client
		pthread_create(&th2,NULL,gestioneClientThread,NULL);
	}//fine while delle connessioni(cioè accetto sempre connessioni)
	return 0;
}

//pone a 0 i punteggi di tutti i client connessi
void azzeraPunteggi(){
	int i;
	for(i=0;i<100;i++)
		punteggioClient[i]=0;

}

void inviaVittoriaTempo(){
	int z;
	int punteggioMax=-1;
	int tempId[100];
	int s=0;

	//trovo il punteggio piu alto al momento
	for(z=0;z<100;z++){
		if(punteggioMax<punteggioClient[z]){
			punteggioMax=punteggioClient[z];
		}
	}

	//trovo tutti i client che hanno il punteggio = a punteggioMax
	for(z=0;z<100;z++){
		if(punteggioClient[z]== punteggioMax){
			tempId[s]=z;
			s++;
		}
	}

	//pongo gli indicatori di vittoria tutti a 3
	for(z=0;z<100;z++){
			 vittoriaVettore[z]=3; //non ha vinto
	}

	for(z=0;z<s;z++){
		vittoriaVettore[ tempId[z] ]=4;	//ha vinto
	}
}

//pongo a 1 gli indicatori di ogni client tranne qll che ha vinto
void inviaVittoria(int id){
	int x;
	for(x=0;x<100;x++){
		if(x==id){
			vittoriaVettore[x]=2;
		}else{
			vittoriaVettore[x]=1;
		}
	}
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

char creaPersonaggio(char m[][MAX]){
	int i;
	int j;	
	int cont = 0;
	char temp;
	while(cont<1){
		i=rand()%(MAX-1)+1;
		j=rand()%(MAX-1)+1;
		if(m[i][j] == '*'){
			int x=0;//
			temp= personaggi[x];
			while( cercaPersonaggio(temp) > 0 ){
				x++;
				temp=personaggi[x];
			}
			cont++;
		}
	}
	return m[i][j]=temp;
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
			if(m[i][j] == personaggio){ 
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

//controllo se è presente tra i personaggi nella matrice il personaggio dato
int cercaPersonaggio(char personaggio){
	int i;
	int j;
	int x=0;
	for(i=1; i<MAX; i++){
		for(j=1; j<MAX; j++){
			if(personaggio == campoGioco[i][j]){
				x=1;
			}
		}
	}
	return x;
}

//qnd viene riavviata una partita viene resettato il campo
void resetPersonaggiConnessi(){
	int i,j;
	int x=0; //contatore dei char dentro personaggi[]
	int z=0; //contatore dei char connessi
	char temp[20];

	//cerco i personaggi ancora connessi (cioè presenti in matrice)
	//e li salvo tutti in un array temporaneo
	for(x=0;x<20;x++){
		for(i=1; i<MAX; i++){
			for(j=1; j<MAX; j++){
				if(campoGioco[i][j]==personaggi[x]){
					temp[z]=personaggi[x];
					z++;
					j=100;
					i=100;
				}
			}
		}
	}
	creaCampo(campoGioco);

	//inserisco i personaggi salvati nel nuovo campo
	for(i=0;i<z;i++){
		int cI,cJ;
		int cont=0;
		do{
			cI=rand()%(MAX-1)+0;
			cJ=rand()%(MAX-1)+0;
			if(campoGioco[cI][cJ] == '*'){
				campoGioco[cI][cJ] = temp[i];
				cont++;
			}
		}while(cont<1);
	}
	creaOstacoli(campoGioco);

	for(i=0;i<z;i++){
		creaPacco(campoGioco);
	}
}

void logging_log(char username[]){
    FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    
    if(fp){
            fprintf(fp, "Login: %s %s\n", username,asctime(localtime(&ora)));
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}

void logging_exit(char username[]){
    FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "Exit: %s %s\n", username,asctime(localtime(&ora)));
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}

void logging_posato(char username[], int i, int j, int punteggio){

	FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "Nome utente: %s Azione: posa pacco Locazione: %d,%d Punteggio: %d\n", username,i,j,punteggio);
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}

void logging_preso(char username[], int i, int j, int punteggio){

	FILE *fp;
    time_t ora;
    ora = time(NULL);
    fp=fopen("logging.txt","a"); //apro il file
    if(fp){
            fprintf(fp, "Nome utente: %s Azione: prendi pacco Locazione: %d,%d Punteggio: %d\n", username,i,j,punteggio);
    }else{
        printf("\nErrore nella scrittura del file!\n");
    }
    fclose(fp);
}

char *salva_username(char username[]){
	int i=0;
	char *ptr;
	char nome[20];
	while(username[i] != '-'){
		username_globali[q] = username[i]; //array di username online

		nome[i] = username[i]; //distinguo l'username dalla password
		q++;
		i++;
	}
	
	nome[i] = '\0';
	username_globali[q] = ',';
	q++;
	username_globali[q] = '\0';
	ptr=nome;
	return ptr;
}

void cancella_online(char username[]){

	int i=0, j=0, tmp;
	int delete = 0;

	while(username_globali[i] != '\0' && delete != 1){ //cancella l'username dalla lista
		if(username_globali[i] == username[j]){
			tmp = i;
		}
		while(username_globali[i] == username[j]){
			i++;
			j++;

			if(username_globali[i] == ','){
				delete = 1;
			}
		}
		i++;
		j=0;
	}

	
	for(i=tmp; username_globali[i] != ','; i++){
		username_globali[i] = '*';
	}

	username_globali[i] = '*';
}
