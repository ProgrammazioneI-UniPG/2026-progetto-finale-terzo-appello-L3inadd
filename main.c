#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

int main() {
	srand((unsigned) time(NULL)); 
	
	int number = 0;
	do {
		do {

			printf("\n==========Menu di gioco==========\n");
			printf(" 1)Imposta gioco \n 2) Gioca \n 3)Termina gioco \n 4)Visualizza i crediti\n");
			printf("===================================\n");
			number = leggi_numero();
			if( number < 1 || number > 4) {
				printf("Valore non valido riprova \n");
			}



		} while (number < 1 || number > 4);

		switch (number)
		{
		case 1:
			imposta_gioco();
			break;
		case 2:
			gioca();
			break;
		case 3:
			termina_gioco();
			break;
		case 4:
			crediti();
			break;

		default:
			break;
		}


	} while (number != 3);

	return 0;

}