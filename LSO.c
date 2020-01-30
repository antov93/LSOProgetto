#include <stdio.h>
#include <stdlib.h>

#define MAX 10

void generazione(int m[][MAX]);
void stampa(int m[][MAX]);
void ostacoli(int m[][MAX]);
void personaggiopacchetto(int m[][MAX]);
int preso(int m[][MAX]);
int ipos(int m[][MAX]);
int jpos(int m[][MAX]);

int main(){
	
	int i;
	int j;
	int m[MAX][MAX];
	char scelta;
	int flag = 0;
	
	generazione(m);
	ostacoli(m);
	personaggiopacchetto(m);
	
	printf("Bisogna muoversi nel labirinto, prendere il pacchetto ed uscire per completare il gioco.\n\nLegenda: \n9-personaggio\n7-Uscita\n1-Spazio libero\n0-Ostacolo\n89-Pacchetto preso\n");

	stampa(m);

	while(m[MAX-1][MAX-1] != 89){

        printf("\nUtilizzare WASD per muovere il personaggio all'interno del labirinto.\n");

       	scanf("%c", &scelta);

		switch(scelta){
			case 'A':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j-1] != 0 && m[i][j-1] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i][j-1] = 9;
					}else{
						m[i][j] = 1;
						m[i][j-1] = 89; 				
					}
					
				}else if(m[i][j-1] == 8){
					m[i][j] = 1;
					m[i][j-1] = 89;					
				}
				stampa(m);
            break;
            
            case 'D':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j+1] != 0 && m[i][j+1] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i][j+1] = 9;
					}else{
						m[i][j] = 1;
						m[i][j+1] = 89; 				
					}
				}else if(m[i][j+1] == 8){
					m[i][j] = 1;
					m[i][j+1] = 89;					
				}
				stampa(m);
            break;

	    case 'W':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i-1][j] != 0 && m[i-1][j] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i-1][j] = 9;
					}else{
						m[i][j] = 1;
						m[i-1][j] = 89; 				
					}
				}else if(m[i-1][j] == 8){
					m[i][j] = 1;
					m[i-1][j] = 89;					
				}
				stampa(m);
            break;

	    case 'S':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i+1][j] != 0 && m[i+1][j] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i+1][j] = 9;
					}else{
						m[i][j] = 1;
						m[i+1][j] = 89; 				
					}
				}else if(m[i+1][j] == 8){
					m[i][j] = 1;
					m[i+1][j] = 89;					
				}
				stampa(m);
            break;
				case 'a':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j-1] != 0 && m[i][j-1] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i][j-1] = 9;
					}else{
						m[i][j] = 1;
						m[i][j-1] = 89; 				
					}
					
				}else if(m[i][j-1] == 8){
					m[i][j] = 1;
					m[i][j-1] = 89;					
				}
				stampa(m);
            break;
            
            case 'd':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i][j+1] != 0 && m[i][j+1] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i][j+1] = 9;
					}else{
						m[i][j] = 1;
						m[i][j+1] = 89; 				
					}
				}else if(m[i][j+1] == 8){
					m[i][j] = 1;
					m[i][j+1] = 89;					
				}
				stampa(m);
            break;

	    case 'w':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i-1][j] != 0 && m[i-1][j] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i-1][j] = 9;
					}else{
						m[i][j] = 1;
						m[i-1][j] = 89; 				
					}
				}else if(m[i-1][j] == 8){
					m[i][j] = 1;
					m[i-1][j] = 89;					
				}
				stampa(m);
            break;

	    case 's':
				i = ipos(m);
				j = jpos(m);
				flag = preso(m);
				if(m[i+1][j] != 0 && m[i+1][j] != 8){
					if(flag == 0){
						m[i][j] = 1;
						m[i+1][j] = 9;
					}else{
						m[i][j] = 1;
						m[i+1][j] = 89; 				
					}
				}else if(m[i+1][j] == 8){
					m[i][j] = 1;
					m[i+1][j] = 89;					
				}
				stampa(m);
            break;
			
				}

	    			
				}
	
printf("\nHAI CONCLUSO LA PARTITA, COMPLIMENTI!\n");


	return 0;
}
	
	


void generazione(int m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			m[i][j] = 1;

			}
		}

}

void ostacoli(int m[][MAX]){
	int i;
	int j;
	int cont = 0;
	int casuale;

	casuale=rand()%20+11;
	while(cont<casuale){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 1){
			m[i][j] = 0;
			cont++;
		}
	}

	m[MAX-1][MAX-1] = 7;
}

void personaggiopacchetto(int m[][MAX]){
	
	int i;
	int j;	
	int cont = 0;

	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 1){
			m[i][j] = 9;
			cont++;
		}
	}

	cont = 0;
	while(cont<1){
		
		i=rand()%(MAX-1)+0;
		j=rand()%(MAX-1)+0;
	
		if(m[i][j] == 1){
			m[i][j] = 8;
			cont++;
		}
	}
	
	
}

int ipos(int m[][MAX]){
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 9 || m[i][j] == 89){
				return i;			
			}

}
}
}


int jpos(int m[][MAX]){
	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 9 || m[i][j] == 89){
				return j;			
			}

}
}
}

int preso(int m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; i<MAX; j++){
			if(m[i][j] == 8){
				return 0;			
			}else if(m[i][j] == 89){
				return 1;
			}
	
}
}
}
 
void stampa(int m[][MAX]){

	int i;
	int j;

	for(i=0; i<MAX; i++){
		for(j=0; j<MAX; j++){
			printf("%d   ", m[i][j]);

			}
			printf("\n");
		}

}
