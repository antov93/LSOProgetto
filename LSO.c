#include <stdio.h>
#include <stdlib.h>

#define MAX 10

//commento###############################

void generazione(char m[][MAX]);
void stampa(char m[][MAX]);
void ostacoli(char m[][MAX]);
void personaggiopacchetto(char m[][MAX]);
int preso(char m[][MAX]);
int ipos(char m[][MAX]);
int jpos(char m[][MAX]);

int main(){
	
	int i;
	int j;
	char m[MAX][MAX];
	char scelta;
	int flag = 0;
	
	generazione(m);
	ostacoli(m);
	personaggiopacchetto(m);
	
	printf("Bisogna muoversi nel labirinto, prendere il pacchetto ed uscire per completare il gioco.\n\nLegenda: \n9-personaggio\n7-Uscita\n1-Spazio libero\n0-Ostacolo\n89-Pacchetto preso\n");

	stampa(m);

	while(m[MAX-1][MAX-1] != 'P'){

        printf("\nUtilizzare WASD per muovere il personaggio all'interno del labirinto.\n");

       	scanf("%c", &scelta);

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
			
				}

	    			
				}
	
printf("\nHAI CONCLUSO LA PARTITA, COMPLIMENTI!\n");


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
