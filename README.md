[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/5fsIc7xe)
# Progetto-finale-2025-Cosestrane
Progetto finale Programmazione Procedurale UniPG Informatica

## Nome: Daniel

## Cognome: Kapllani

## Matricola:390477

## Commenti/modifiche al progetto:
Nel File gamelib.h:
-Aggiunta di variabili all interno della Struct Giocatore per gestire la sua vita, le stats basi per dei possibili potenziamenti durante i vari turni e combattimenti dei giocatori e movimenti etra che può fare interagendo con alcuni oggetti dati dal file del progetto.

-Creazione di una Struct identificata come Esito_combattimento per controllare lo stato del giocatore durante il combattimento in caso muoia, vinca, uccide il boss oppure non succeda niente.


Nel File gamelib.c
-Gestione della lettura degli Input attraverso due funzioni leggi_numero leggi_stringa per evitare i doppi invii e i possibili crash.

-Gestione delle statistiche del giocatore attraverso la funzione modifica_attributi che permette di scambiare attacco e difesa come scritto da progetto entro certi limiti.

-Creazione di tre file nominate nome_oggetto, nome_zona, nome_nemico cosi che durante il gioco l'utente possa vedere cosa ci sia in quella zona: ovvero il nome della zona l'oggetto al suo interno e il possibile mostro che dovrà affrontare cosi da risolvere la stampa dei numeri della struct in cui sono disposti i vari valori.

-conta_demotorzone per verificare che ci sia un singolo demotorzone o che sia stato ucciso attraverso altre funzioni.

-conta_zone_mr utilizzato per vedere qaunte zone ci siano nella mappa in caso poi l'utente decida di aggiungere o rimuovere delle celle e pe muoversi meglio all interno dei cicli for delle varie funzioni.

-conta_speciale controlla se esita gia uno speciale tra i vari giocatori in caso qualcuno volesse cambiare le stats e diventare anche lui UnidicivirgolaCinque.

-genera_ordine_casuale per gestire l'ordine casuale dei giocatori per ogni turno.

-tutti_giocatori_morti per controllare se i giocatori siano morti prima che continui il turno successivo cosi da concludere la pratita.

-pulitore_morti pulisce l'array di giocatori togliendo quelli che hanno giocati ma sono morti.

-zona_ha_nemico controlla se nel Mondo Reale o nel Soprasotto ci siano nemici per gestire poi i turni dei giocaotori.

-lancio_dado usato sia per le statistiche del giocatore e nel combattimento sia per attaccare sia per la fuga.

-crea_giocatore usato per creare il giocatore e inserire tutti i valori nella struct  sia quelli del progetto e quelli extra.

-inizalizza_giocatori dove l'inserisce nel array da 4 dei massimi giocatori che possono giocare.

-descrivi_oggetti fornisce una panoramica di cosa fanno gli oggetti.

-stampa_giocatori stampa tutte le specifiche del giocatore.

-modifica_attribuiti da la possibilità al giocatore di cambiare le statistiche.

-libera_mappa_zonaReale elimina le celle delle caselle del Mondo Reale.

-libera_mappa_sopraSotto elimina le celle delle caselle del Soprasotto.
 
-pulitore_mappa usa le funzioni precedentemente descitte per poi pulire completamente la mappa e in caso il giocatore vole creare di nuovo la mappa.

-genera_reale creazione di una cella del Mondo Reale con tutto ciò che deve avere descritto dal progetto. -usato all' interno di genera_mappa.

-genera_sotto creazione di una cella del Sottosopra con tutto ciò che deve avere descritto dal progetto.   -usato all' interno di genera_mappa.

-libera_giocatori rimuovere i giocatori alla fine del gioco.

-menu_genera_mappa permette di decidere tra le funzioni del progetto, imposta-gioco.

-usa_bussola permette di usare la bussola overe vedere cose ce nella porssima cella siadel Mondo Reale e del SopraSotto cosi da potre decidere quale prendere.

-MODIFICA ALLA FUNZIONE raccogli_oggetto del progetto implementando all utente di poter lasciare (droppare) un oggetto in caso lui abbia gia lo zaino pieno e di lasciare uno dei sui 3 oggetti in quella zona cosi in caso un altro player passa li potrà prenderlo.

-reset_stat_giocatore resetta le statistiche del giocatore dopo aver utilizzato un oggetto in un turno del combattimento.

-turno_giocatore menu che permette al giocatore di decidere una delle opzioni disponibili ovvero delle funzioni del progetto di gioca.

-aggiorna_giocatore aggiorna il vincitore di una partita quando si guardano i crediti oppure finisce il gioco.

## Istruzioni per la Compilazione ed Esecuzione
Il progetto è stato sviluppato seguendo lo standard C11 e testato su ambiente Linux (Debian).

Per compilare il progetto:
gcc main.c gamelib.c -Wall -std=c11 -o gioco

Per eseguire il programma:
./gioco
