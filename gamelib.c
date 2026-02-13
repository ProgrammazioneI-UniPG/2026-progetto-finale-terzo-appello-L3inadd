#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_VINCITORI 3

static char ultimi_vincitori[MAX_VINCITORI][50];
static Giocatore* giocatori[4] ={NULL, NULL, NULL, NULL};
static int numero_giocatori=0;
struct Zona_mondoReale* prima_zona_mondoReale = NULL;
struct Zona_sopraSotto* prima_zona_sopraSotto = NULL;
static int mappa_chiusa =0;
static int gioco_finito = 0;


/* ================= INPUT ================= */

int leggi_numero() {
	char buffer[100];
    int numero;
    if(fgets(buffer,sizeof(buffer), stdin) == NULL){
        return -1;
    }

    if(buffer[0] == '\n'){
        return -1;
    }

    if(strchr(buffer, '\n') == NULL){
        int c;
        while((c=getchar() != '\n') && c!= EOF);
    }

    if(sscanf(buffer,"%d", &numero) != 1){
        return -1; //se scrivo "abc" ,restitusice -1 invece di 0
    }
	return atoi(buffer);
}


static void leggi_stringa(char* buffer, int size) {
    // Continua a leggere finché non trova una riga che non sia solo un "Invio"
    do {
        if (fgets(buffer, size, stdin) == NULL) {
            break; 
        }
        // Rimuove il carattere newline (\n) alla fine
        buffer[strcspn(buffer, "\n")] = '\0';

    } while (buffer[0] == '\0'); // Se la stringa è vuota, riprova
}


/*=================== OGGETTI=====================*/

static const char* nome_oggetto(Tipo_oggetto oggetto) {
    switch(oggetto) {
        case NESSUN_OGGETTO:
            return "Vuoto";

        case BICICLETTA:
           return "Bicicletta";

        case CHITARRA_METALLICA: 
            return "Chitarra Metallica";

        case BUSSOLA:             
            return "Bussola";
        
        case MAGLIETTA_FUOCOINFERNO:              
            return "Maglietta fuoco inferno";

        default:                
            return "Vuoto";
    }
}



/*=================== ZONE =====================*/
static const char* nome_zona(Tipo_zona zona) {
    switch(zona) {
        case BOSCO:                
            return "Bosco";

        case SCUOLA:               
            return "Scuola";

        case LABORATORIO:          
            return "Laboratorio";

        case CAVERNA:              
            return "Caverna";

        case STRADA:               
            return "Strada";

        case GIARDINO:             
            return "Giardino";

        case SUPERMERCATO:         
            return "Supermercato";

        case CENTRALE_ELETTRICA:   
            return "Centrale Elettrica";

        case DEPOSITO_ABBANDONATO: 
            return "Deposito Abbandonato";

        case STAZIONE_POLIZIA:     
            return "Stazione di Polizia";

        default:                   
            return "Zona Ignota";
    }
}

/*=================== NEMICO =====================*/

static const char* nome_nemico(Tipo_nemico nemico) {
    switch(nemico) {
        case NESSUN_NEMICO: 
            return "Nessun nemico";

        case BILLI:         
            return "Billi";

        case DEMOCANE:      
            return "Democane";

        case DEMOTORZONE:   
            return "IL DEMOTORZONE"; // Scritto in grande perché è il boss!

        default:                
            return "Nessun nemico";
    }
}


/* ================= CONTATORI ================= */

static int conta_demotorzone(){
    int count = 0;
    Zona_sopraSotto* controllore = prima_zona_sopraSotto;
    while(controllore != NULL){
        if(controllore->nemico == DEMOTORZONE ){
            count++;
        }
        controllore = controllore->avanti;
    }
    return count;
}

static int conta_zone_mr(){
    int count =0;
    Zona_mondoReale* cella = prima_zona_mondoReale;
    
    while(cella != NULL){
        count++;
        cella = cella->avanti;
    }
    return count;
}

static int conta_speciale(){
    int numero_speciale=0;
    Giocatore* g;
    for(int i=0; i< numero_giocatori; i++){
        g = giocatori[i];
        if(giocatori[i] != NULL && giocatori[i]->speciale == 1){
            printf("%s e gia il numero speciale \n",g->nome);
            numero_speciale = 1;
            return numero_speciale;
        }
    }
    printf("---nome speciale libero---\n");
    return numero_speciale;
}

static void genera_ordine_casuale(int ordine[]){
    for( int i=0; i< numero_giocatori;i++){
            ordine[i]= i;
        }

        //mischio ordine
    for(int i=0;i< numero_giocatori; i++){
        int j = rand()% numero_giocatori;
        int tmp = ordine[i];
        ordine[i] = ordine[j];
        ordine[j] = tmp;
    }
}

static int tutti_giocatori_morti(){
    for(int i=0; i<numero_giocatori; i++){
        if(giocatori[i] && giocatori[i]->vivo){
            return 0;
        }
    }
    return 1;
}

static void pulischi_morti(){
    for(int i=0; i >numero_giocatori; i++){
        if(giocatori[i] != NULL && giocatori[i]->vivo == 0){
            printf("Rimozione definitiva di %s dal campo di gioco...\n", giocatori[i]->nome_originale);
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
}

static int zona_ha_nemico(Giocatore* g) {
    if (g->mondo == 0){
        return g->pos_mondoreale->nemico != NESSUN_NEMICO;
    }
    else{
        return g->pos_soprasotto->nemico != NESSUN_NEMICO;
    }
}


/* ================= UTILITA ================= */
static int lancio_dado(int facce){
    return ( rand() %  facce + 1 );
}


/* ================= GIOCATORE ================= */
static Giocatore* crea_giocatore(){
    Giocatore* g = malloc(sizeof(Giocatore));
    char buffer[20];
    if(g == NULL){
        printf("errore dureante la locazione della meroria per il giocatore. \n");
        exit(1);
    }
    //nome
    printf("\n==============================\n");
    printf("   CREAZIONE NUOVO GIOCATORE\n");
    printf("==============================\n");
    
    printf("Ineserici il nome del giocatore: ");
    leggi_stringa(buffer, 19);
    strcpy(g->nome, buffer);
    strcpy(g->nome_originale, buffer);
    g-> hp = 75;
    g-> vivo = 1;
    g->speciale = 0;

    //reset
    g->att_base  = 0;
    g->dif_base  = 0;
    g->fort_base = 0;


    //stats
    g-> attacco_psichico = lancio_dado (20);
    g-> difesa_psichica = lancio_dado(20);
    g-> fortuna = lancio_dado(20);
    //salvatggio stats per modifiche future 
    g->att_base  = g->attacco_psichico;
    g->dif_base  = g->difesa_psichica;
    g->fort_base = g->fortuna;

    
    printf("\nStatistiche iniziali:\n");
    printf("------------------------------\n");
    printf("Attacco psichico : %2d\n", g->attacco_psichico);
    printf("Difesa psichica  : %2d\n", g->difesa_psichica);
    printf("Fortuna          : %2d\n", g->fortuna);
    printf("------------------------------\n");
    //zaino
    for ( int j=0; j<3; j++){
        g-> zaino[j] = NESSUN_OGGETTO;
    }

    g-> pos_mondoreale = NULL;
    g-> pos_soprasotto = NULL;
    g-> mondo = 0;
    return g;
}




static void inizializza_giocatori(){
    for(int i=0; i<numero_giocatori; i++){
        giocatori[i] -> mondo = 0;
        giocatori[i] -> pos_mondoreale = prima_zona_mondoReale;
        giocatori[i] -> pos_soprasotto = prima_zona_sopraSotto;
    }
}

/* ================= STAMPA ================= */

static void descrivi_oggetto(Tipo_oggetto o){
    switch(o){
        case BICICLETTA:
            printf("Bicicletta: ti permette di muoverti per un turno extra. \n");
            break;

        case MAGLIETTA_FUOCOINFERNO :
            printf("Maglietta infuocata: 21 di difesa . \n");
            break;

        case BUSSOLA:
            printf("Bussola: mostra informazioni sulla zona corrente. \n");
            break;

        case CHITARRA_METALLICA:
            printf("Chitarra metallica: infligge 30 danni al mostro. \n ");
            break;
            
        default:
            printf("Nessun oggetto\n");
    }
}

void stampa_giocatore(Giocatore* g) {
    if (g == NULL) {
        printf("Giocatore non valido.\n");
        return;
    }

    printf("\n===== STATO GIOCATORE =====\n");

    printf("Nome: %s\n", g->nome);
    printf("Nome originale: %s\n", g->nome_originale);

    printf("Stato: %s\n", g->vivo ? "Vivo" : "Morto");
    printf("HP: %d\n", g->hp);
    printf("Speciale: %d\n", g->speciale);

    printf("Mondo: %s\n", g->mondo == 0 ? "Mondo Reale" : "Soprasotto");

    if (g->mondo == 0 && g->pos_mondoreale != NULL) {
        printf("Zona attuale (Mondo Reale): %s\n", nome_zona(g->pos_mondoreale->zona));
        printf("Nemico nella zona: %s\n", nome_nemico(g->pos_mondoreale->nemico));
        printf("Oggetto nella zona: %s\n", nome_oggetto(g->pos_mondoreale->oggetto));
    }
    else if (g->mondo == 1 && g->pos_soprasotto != NULL) {
        printf("Zona attuale (Soprasotto): %s\n", nome_zona(g->pos_soprasotto->zona));
        printf("Nemico nella zona: %s\n", nome_nemico(g->pos_soprasotto->nemico));
    }

    printf("\n--- Statistiche ---\n");
    printf("Attacco psichico: %d (base %d)\n", g->attacco_psichico, g->att_base);
    printf("Difesa psichica: %d (base %d)\n", g->difesa_psichica, g->dif_base);
    printf("Fortuna: %d (base %d)\n", g->fortuna, g->fort_base);

    printf("\n--- Zaino ---\n");
    for (int i = 0; i < 3; i++) {
        printf("Slot %d: %s\n", i + 1, nome_oggetto(g->zaino[i]));
        descrivi_oggetto(g->zaino[i]);
    }

    printf("===========================\n\n");
}


/* ================= MODIFICHE ================= */

static void modifica_attributi(Giocatore* g) {
    int scelta;

    printf("\n----- MODIFICA ATTRIBUTI -----\n");
    printf("0) Esci\n");
    printf("1) +3 Attacco / -3 Difesa\n");
    printf("2) -3 Attacco / +3 Difesa\n");

    if (g->speciale == 0 && conta_speciale() == 0) {
        printf("3) Diventa UndiciVirgolaCinque\n");
    }

    if (g->speciale == 1) {
        printf("4) Torna normale\n");
    }

    printf("Scelta: ");
    scelta = leggi_numero();

    switch (scelta) {

    case 0:
        return;

    case 1:
        if (g->dif_base < 4) {
            printf("Difesa troppo bassa.\n");
            break;
        }

        g->att_base += 3;
        g->dif_base -= 3;

        g->attacco_psichico = g->att_base;
        g->difesa_psichica  = g->dif_base;
        g->fortuna = g->fort_base;

        return;

    case 2:
        if (g->att_base < 4) {
            printf("Attacco troppo basso.\n");
            break;
        }

        g->att_base -= 3;
        g->dif_base += 3;

        g->attacco_psichico = g->att_base;
        g->difesa_psichica  = g->dif_base;
        g->fortuna = g->fort_base;
            
        return;

    case 3:
        if(g->fort_base < 8){
            printf("Fortuna base troppo bassa.\n");
            break;
        }
        if (g->speciale == 0 && conta_speciale() == 0) {
            g->att_base += 4;
            g->dif_base += 4;
            g->fort_base -= 7;
            g->speciale = 1;

            g->attacco_psichico = g->att_base;
            g->difesa_psichica  = g->dif_base;
            g->fortuna = g->fort_base;
                strcpy(g->nome, "UndiciVirgolaCinque");
        } else {
            printf("Non puoi diventare speciale.\n");
        }
        return;

    default:
        printf("Scelta non valida.\n");
        return;
    }
}



/* ================= PULITORE MAPPA ================= */


static void libera_mappa_zonaReale(){
    Zona_mondoReale* zona = prima_zona_mondoReale;
    Zona_mondoReale* tmp;

    while(zona !=NULL){
        tmp = zona;
        zona = zona->avanti;
        free(tmp);
    }
    prima_zona_mondoReale =NULL; 
}


static void libera_mappa_sopraSotto(){
    Zona_sopraSotto* zona = prima_zona_sopraSotto;
    Zona_sopraSotto* tmp;

    while(zona !=NULL){
        tmp = zona;
        zona = zona->avanti;
        free(tmp);
    }
    prima_zona_sopraSotto =NULL; 
}

static void pulitore_mappa(){
   libera_mappa_zonaReale();
   libera_mappa_sopraSotto();
}


/* ================= GENERATORE MAPPA ================= */

static Zona_mondoReale* genera_reale(){
    Zona_mondoReale* casella = malloc(sizeof(Zona_mondoReale));
    casella -> zona = rand() % 10;
    casella -> nemico= rand() % 3; // billi, nessun_ nemico, democane 
    casella -> oggetto = rand() % 5;
    casella -> avanti = NULL;
    casella -> indietro = NULL;
    casella ->  link_sopraSotto = NULL;
    return casella;
}


static Zona_sopraSotto* genera_sotto(){
    Zona_sopraSotto* casella = malloc(sizeof(Zona_sopraSotto));
    casella -> zona = rand() % 10;
    casella ->nemico = rand() % 2; // nessuno, democane, demotorzone SOLO UNO ---aggiungerlo forzatamente--
    casella -> avanti = NULL;
    casella -> indietro = NULL;
    casella -> link_mondoReale = NULL;
    return casella;
}




static void genera_mappa(){

    pulitore_mappa();
    

    int posizione = rand() % 15 ;
    Zona_mondoReale* cella_mr = NULL;
    Zona_sopraSotto*cella_ss = NULL;
    for(int i=0; i<15; i++){
        Zona_mondoReale* zmr = genera_reale();
        Zona_sopraSotto* zss = genera_sotto();

        zmr -> link_sopraSotto = zss;
        zss -> link_mondoReale = zmr;

        //collegamento lista doppiamente collegata 
        if(cella_mr){
            cella_mr -> avanti = zmr;
            zmr -> indietro = cella_mr;
        } else{
            prima_zona_mondoReale = zmr; //prima zona 
        }
        cella_mr= zmr;

        // collegamento sottoSopra 
        if( !conta_demotorzone() && i == posizione) {
            zss-> nemico = DEMOTORZONE ;
        }

        if(cella_ss){
            cella_ss ->avanti = zss;
            zss -> indietro = cella_ss;
        }else{
            prima_zona_sopraSotto = zss;
    
        }
        cella_ss = zss;
    }
    if(conta_demotorzone() != 1){
        printf("Errore: numero errato di demonitrozone!\n");
    }
    
    
    printf("sono state generate 15 zone per il Mondo Reale e il Sotto Sopra. \n");
}


/* =================INSERISCI ZONA ================= */


static void inserisci_zona(){

    int n_zone = conta_zone_mr();
    int posizione =-1;
    int nemico_mr =0; 
    int nemico_ss = 0;
    int item =0;

    do{
        printf("Inserisci la posizione da aggiungere(0 =%d):", n_zone);
        posizione = leggi_numero();
        printf("\n");
    }while(posizione<0 || posizione > n_zone );

    Zona_mondoReale* zmr = genera_reale();
    Zona_sopraSotto* zss = genera_sotto();

    zmr -> link_sopraSotto = zss;
    zss -> link_mondoReale = zmr;

    do{
        printf("=====Inserisci il nemico per il Mondo reale=====\n 0)Nessun nemico \n 1)Demone cane \n 2)Billi\n");
        nemico_mr = leggi_numero();
        printf("\n");
    }while(nemico_mr < 0 || nemico_mr > 2 );
    zmr->nemico = nemico_mr;

    do{
        printf("===Oggetti===\n 0)Nessun oggetto\n  1)Bicicletta\n 2)Maglietta fuocoinferno\n 3)Bussola\n 4)Schitarrata metallica\n");
        item = leggi_numero();
        printf("\n");
    }while(item < 0 || item > 4 );
    zmr->oggetto = item;

    do{
        printf("====Inerici il nemico per il Soprasotto=====\n 0)Nessun nemico \n 1) Democane\n 2)Demotorzone\n");
        nemico_ss = leggi_numero();
        printf("\n");
        if(nemico_ss == 2 && conta_demotorzone() >=1 ){
            printf("Errore: esiste gia un demotorzone. \n");
            nemico_ss = -1;
        }
        // controllare se esiste gia il demotorzone
    }while(nemico_ss < 0 || nemico_ss > 2);
    if(nemico_ss == 2){
        nemico_ss = 3;
    }
    zss->nemico = nemico_ss;


    //inserimento nella lista 
    if(posizione == 0){   
         //solo per pos. 0
        zmr->avanti = prima_zona_mondoReale;
        if(prima_zona_mondoReale){
            prima_zona_mondoReale->indietro = zmr;
        }
        prima_zona_mondoReale = zmr;

        zss->avanti = prima_zona_sopraSotto;
        if(prima_zona_sopraSotto){
            prima_zona_sopraSotto -> indietro = zss;
        }
        prima_zona_sopraSotto = zss;
     }else{ 
           //tutte le altre 
        Zona_mondoReale* cella_mr= prima_zona_mondoReale;
        Zona_sopraSotto* cella_ss= prima_zona_sopraSotto;
        for(int i=0; i<posizione-1; i++){
            cella_mr = cella_mr->avanti;
            cella_ss = cella_ss-> avanti;
        }

        zmr->avanti=cella_mr->avanti;
        zmr->indietro = cella_mr;
        if(cella_mr->avanti){
            cella_mr->avanti->indietro =zmr;
        }
        cella_mr -> avanti = zmr;

        zss->avanti=cella_ss->avanti;
        zss->indietro=cella_ss;
        if(cella_ss->avanti){
            cella_ss ->avanti-> indietro = zss;
        }
        cella_ss->avanti=zss;
     }
     printf("Zona inserita\n");
}



/* ================= DEALLOCA GIOCATORE ================= */
static void libera_giocatori(){
    for(int i=0; i<numero_giocatori; i++){
        if(giocatori[i] != NULL){
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }

    numero_giocatori=0;
}



/* ================= CANCELLAZIONE ZONA ================= */


static void cancella_zona(){
     if(prima_zona_mondoReale == NULL){
        printf("La mappa è vuota nulla da eliminare.\n");
        return;
    }
    int pos =-1;
    int n_zona = conta_zone_mr();

    do{
        printf("Iserisci la poszione da cancellare da 0 a %d\n", n_zona-1);
        pos = leggi_numero();
        if(pos<0 || pos > conta_zone_mr()){
            printf("Casella non compresa nel range 0-%d o valore errato \n", n_zona-1);
        }
    }while(pos<0 || pos> n_zona-1);


    //caso 0

    if(pos == 0){
        Zona_mondoReale* corrente_mr = prima_zona_mondoReale;
        Zona_sopraSotto* corrente_ss = prima_zona_sopraSotto;

        if(corrente_ss-> nemico == DEMOTORZONE){
            printf("Non puoi eliminare la zona con il demotorzone \n");
            return;
        }
        prima_zona_mondoReale= corrente_mr->avanti;
        prima_zona_sopraSotto = corrente_ss->avanti;

        if(prima_zona_mondoReale != NULL){
            prima_zona_mondoReale -> indietro = NULL;
        }

        if(prima_zona_sopraSotto != NULL){
            prima_zona_sopraSotto->indietro = NULL;
        }

        free(corrente_mr);
        free(corrente_ss);
        return;
    }

    // tutti gli altri casi != 0

    Zona_mondoReale* corrente_mr = prima_zona_mondoReale;
    Zona_sopraSotto* corrente_ss = prima_zona_sopraSotto;
    for(int i=0; i<pos; i++){
        corrente_mr = corrente_mr -> avanti;
        corrente_ss = corrente_ss -> avanti;
    }

    //controllo del demotorzone in quella cella 
    if(corrente_ss-> nemico == DEMOTORZONE && conta_demotorzone() ==1 ){
        printf("non puoi cancellare l'unico demotrozone\n");
        return;
    }

    corrente_mr->indietro -> avanti =corrente_mr -> avanti;
    if(corrente_mr -> avanti != NULL){
        corrente_mr->avanti -> indietro = corrente_mr ->indietro;
    }
    

    corrente_ss->indietro -> avanti = corrente_ss -> avanti;
    if(corrente_ss -> avanti != NULL){
        corrente_ss->avanti -> indietro = corrente_ss->indietro;
    }
    
    free(corrente_mr);
    free(corrente_ss);
}


/* ================= STAMPA ZONA E MAPPA ================= */

static void stampa_mappa(){
//controllo  di sicurezza
if (prima_zona_mondoReale == NULL){
    printf("\n ERRORE: La mappa e vuota. devi prima gnerala \n");
}
    int scelta = 0;
    do{
        printf("cosa vuoi stampare? 1)Modno Reale\n 2)SopraSotto\n");
        scelta = leggi_numero();
        if(scelta < 1 ||  scelta > 2){
            printf("Input errato si preg di riprovare\n");
        }
    }while(scelta < 1 || scelta >2);

    if(scelta ==1){
        int i=0;
        Zona_mondoReale* cella_mr = prima_zona_mondoReale;
        while(cella_mr != NULL){
            printf("===========================");
            printf("Zona %d (Mondo Reale)\n", i);
            printf("Tipo di zona: %s\n", nome_zona(cella_mr->zona));
            printf("Nemico: %s\n", nome_nemico(cella_mr-> nemico));
            printf("oggetto: %s\n", nome_oggetto(cella_mr -> oggetto));
            printf("===========================");
            printf("\n");

            cella_mr = cella_mr -> avanti;
            i++;
        }
    }else{
        Zona_sopraSotto* cella_ss = prima_zona_sopraSotto;
        int i =0;
        while(cella_ss != NULL){
            printf("===========================");
            printf("Zona %d (Soprasotto)\n", i);
            printf("Tipo di zona: %s\n", nome_zona(cella_ss->zona));
            printf("Nemico: %s\n", nome_nemico(cella_ss-> nemico));
            printf("===========================");
            printf("\n");

            cella_ss = cella_ss -> avanti;
            i++;
        }
    }
}


static void stampa_zona(){
    if(prima_zona_mondoReale == NULL){
        printf("ERROR: la mappa non è stata ancora gnerata!\n");
        return;
    }
    int n_zona = conta_zone_mr();
    int scelta=0;

    do{
        printf("quale casella vuoi stampare della zona del Mondo Reale? ( da 0 a %d)", n_zona -1);
        scelta = leggi_numero();
        printf("\n");

        if(scelta < 0 || scelta > n_zona -1){
            printf("Errore durante l'input \n");  
        }

    }while(scelta < 0 || scelta > n_zona -1);

    Zona_mondoReale* casella_mr = prima_zona_mondoReale;

    for(int i=0; i < scelta && casella_mr != NULL; i++){
        casella_mr = casella_mr -> avanti; 
    }

    if(casella_mr == NULL){
        printf("ERRORE IMPREVISTO: ZONA NON TROVATA\n");
        return;
    }

    printf("--- ZONA MONDO REALE--- \n");
    printf("Tipo zona: %s \n", nome_zona(casella_mr->zona));
    printf("Nemico: %s \n ", nome_nemico(casella_mr ->nemico));
    printf("Oggetto: %s \n", nome_oggetto(casella_mr -> oggetto));

    if(casella_mr->link_sopraSotto != NULL){
        Zona_sopraSotto* casella_ss = casella_mr -> link_sopraSotto;
        printf("--- ZONA SOPRASOTTO---\n");
        printf("Tipo zona: %s \n", nome_zona(casella_ss-> zona));
        printf("Nemico: %s\n", nome_nemico(casella_ss -> nemico));
    }else{
        printf(" AVVISO qusta zona non ha il collegamento al soprasotto \n");
    }

    printf("-----------------------------------\n");
}



static void stampa_zona_giocatore(Giocatore* g){
    if(g->mondo == 0){
        Zona_mondoReale* zmr = g->pos_mondoreale;
        printf("===MONDO REALE ===\n");
        printf("Zona: %s\n", nome_zona(zmr->zona));
        printf("Nemico: %s\n", nome_nemico(zmr->nemico));
        printf("Oggetto: %s\n", nome_oggetto(zmr->oggetto));
        printf("==================\n");
    }else{
        Zona_sopraSotto* zss = g->pos_soprasotto;
        printf("===SOPRASOTTO=== \n");
        printf("Zona: %s \n", nome_zona(zss->zona));
        printf("Nemico: %s \n", nome_nemico(zss-> nemico));
        printf("==================\n");
    }
}

/* ================= CHIUSURA MAPPA ================= */

static void chiudi_mappa(){
    int zona = conta_zone_mr();
    int demone = conta_demotorzone();

    if(zona < 15){
        printf("Errore la mappa a meno di 15 zone \n");
        return;
    }

    if(demone != 1){
        printf("Errore la mappa deve avere SOLO UN Demotrozone!\n");
        mappa_chiusa = 0;
        return;
    }

    mappa_chiusa = 1 ; 
    printf("Mappa chiusa. Il gioco puo inizare \n");
}

/* ================= MENU MAPPA ================= */

static void menu_genera_mappa(){
    int scelta;
    do{
        do{
            printf("\n===== MENU CREAZIONE MAPPA =====\n");
            printf("1) Genera nuova mappa (15 zone)\n");
            printf("2) Inserisci zona\n");
            printf("3) Cancella zona\n");
            printf("4) Stampa mappa\n");
            printf("5) Stampa zona\n");
            printf("6) Chiudi mappa\n");
            printf("Scelta: ");
            scelta = leggi_numero();
            if(scelta< 1 || scelta >6){
                printf("Valore inesestente riporva");
            }
        }while(scelta< 1 || scelta >6);

        switch(scelta){

            case 1:
                genera_mappa();
                break;

            case 2:
                inserisci_zona();
                break;

            case 3:
                cancella_zona();
                break;

            case 4:
                stampa_mappa();
                break;
            
            case 5:
                stampa_zona();
                break;

            case 6:
                chiudi_mappa();
                break;

            default:
                printf("valore non valido");
        }   
    }while(scelta !=6);   
}

/* ================= IMPOSTA GIOCO ================= */

void imposta_gioco(){
    pulitore_mappa();
    libera_giocatori();
    char buffer[20];
    
    do{
        printf("\n\n");
        printf(" Numero di giocatori (1 - 4): ");
        numero_giocatori = leggi_numero();
        if((numero_giocatori < 1) || (numero_giocatori >4)){
        printf("Inseririe un numero valido per giocare (tra 1 e 4) \n");
        }
        printf("\n\n");
    }while(numero_giocatori < 1 || numero_giocatori > 4 );

    for(int i=0; i<numero_giocatori; i++){
        printf("\n>>> Giocatore %d <<<\n", i + 1);
        giocatori[i] = crea_giocatore();
        char modifica;
        int scelta =1;

        do{
            printf("Vuoi modificare i valori del giocatore appena creato ? y/n: ");
            leggi_stringa(buffer, 20);
            modifica = buffer[0];
            if( modifica == 'y' ){
                if(scelta){
                    printf("MODIFICA \n");
                    modifica_attributi(giocatori[i]);
                    scelta=0;
                }else{
                    printf("Hai gia cambiato le statistiche sei un  beater \n");
                }
                
            }else if(modifica == 'n'){
                printf("ok\n");
                break;
            }else{
                printf("Carattere non vailido perfavore inserire y o n. \n");
            }

        }while( modifica != 'n');
        stampa_giocatore(giocatori[i]);
    }

    do{
        menu_genera_mappa();
    }while(!mappa_chiusa);

}




/* ================= MOVIMENTO GIOCATORE================= */

static void avanza(Giocatore* g){
    if(g-> mondo == 0){
        if(zona_ha_nemico(g)){ // controllo se nella cella ci sia un nemico 
            printf("devi prima sconfiggere il nemico di questa zona \n");
            return;
        }

        if(g->pos_mondoreale->avanti == NULL){
            printf("sei gia nel'ultima zona del Mondo Real!\n");
            return;
        }
        
        g->pos_mondoreale = g->pos_mondoreale-> avanti;
        printf("%s ti sei mosso nella zona succesica del Mondo Reale \n", g->nome);

    }else{
        if(zona_ha_nemico(g)){
            printf("devi combattere il nemico di questa zona \n");
            return;
        }
        if (g->pos_soprasotto->avanti == NULL){
            printf("sei arrivato all'ultima zona del Soprasotto\n");
            return;
        }

        g->pos_soprasotto = g-> pos_soprasotto -> avanti;
        printf("%s ti sei mosso nella zona succesica del Soprasotto \n", g->nome);
    }
}


static void indietreggia(Giocatore *g){
    if(g->mondo == 0){
        if(zona_ha_nemico(g)){
            printf("Devi combattere il nemico di questa zona prima di tornare indietro \n");
            return;
        }

        if(g->pos_mondoreale->indietro == NULL) {
            printf("sei gi alla prima zona dell Mondo Reale\n");
            return;
        }

        g->pos_mondoreale = g->pos_mondoreale->indietro;

    }else{
        if(zona_ha_nemico(g)){
            printf("Devi combattere il nemico di questa zona prima di tornare indietro\n");
            return;
        }

        if(g->pos_soprasotto->indietro == NULL){
            printf("sei gia alla prima zona del Soprasotto \n");
            return;
        }

        g->pos_soprasotto = g->pos_soprasotto->indietro;
        printf("%s è tonato alla on precedente del SopraSotto", g->nome); 
    }
}


static void cambia_mondo(Giocatore* g, int* avanza_usato){

    if (*avanza_usato){
        printf("Non puoi cambiare mondo essendo che hai gia avanzato in questo turno \n");
        return;
    }

    // ===== DAL MONDO REALE AL SOPRASOTTO =====
    if(g->mondo == 0){
        if(zona_ha_nemico(g)){
            printf("Devi prima sconfiggere il nemico nel Mondo Reale per cambiare mondo \n");
            return;
        }

        g->mondo=1;
        g->pos_soprasotto = g->pos_mondoreale -> link_sopraSotto ;
        *avanza_usato=1;
        printf("%sè passato nel Soprasotto \n", g->nome);
        return;

    }

    // ===== DAL SOPRASOTTO AL MONDO REALE =====

    if(zona_ha_nemico(g)){
        int decisione;
        do{
            printf("devi prim socnfiggere il nemico oppure poi scapare lanciando il dadoper tornare nel Mondo Reale \n 1) Combatti \n 2) Scappare\n");
            decisione = leggi_numero();
            if(decisione <1 || decisione > 2){
                printf("Valore non valido riprova\n");
            }
        }while(decisione < 1 || decisione >2);

        if(decisione == 1){
            return;
        }

        int dado = lancio_dado(20);
        printf("%s lancia il dado per tronare nel mondo reale %d VS %d \n", g-> nome, dado,g->fortuna);
        
        if(dado >= g->fortuna){
            printf("Tentatico Fallito! Devi combattere. \n");
            return;
        }
    } 

    g->mondo =0;
    g->pos_mondoreale = g->pos_soprasotto->link_mondoReale;
    *avanza_usato = 1;

    printf("%s è tornato nel mondo reale. \n", g->nome);
}




/* ================= OGGETTO================= */

static void usa_bussola(Giocatore* g){
    if(g== NULL){
        return;
    }

    printf("===USO BUSSOLA === \n");

    //zona corrente
    if(g->mondo == 0){
        Zona_mondoReale* z_corrente = g->pos_mondoreale; 
        if(z_corrente->avanti){
            printf("\n============================================================\n");
            printf("prossima zona del Mondo Reale: Zona: %s \n, Nemico: %s \n  Oggetto: %s \n", nome_zona(z_corrente->avanti->zona), nome_nemico(z_corrente->avanti->nemico), nome_oggetto(z_corrente->avanti->oggetto));
            if(z_corrente->avanti->link_sopraSotto){
                printf("Prossima zona soprasotto: Zona: %s \n Nemico: %s \n" , nome_zona(z_corrente->avanti->link_sopraSotto->zona), nome_nemico(z_corrente->avanti->link_sopraSotto->nemico));
                 printf("\n============================================================\n");
            }
        }else{
            printf("non ci sono zone succesive. \n");

        }
    }else{
        Zona_sopraSotto* z_corrente = g->pos_soprasotto;
        if(z_corrente->avanti){
             printf("\n============================================================\n");
            printf("prossima zona del Mondo Reale: Zona: %s\n Nemico: %s \n", nome_zona(z_corrente->avanti->zona), nome_nemico(z_corrente->avanti->nemico));
            if(z_corrente->avanti->link_mondoReale){
                printf("Prossima zona soprasotto: Zona: %s \n Nemico: %s \n", nome_zona(z_corrente->avanti->link_mondoReale->zona), nome_nemico(z_corrente->avanti->link_mondoReale->nemico));
                 printf("\n============================================================\n");
            }
        }else{
            printf("non ci sono zone succesive. \n");
        }
    }
}





static void raccogli_oggetto(Giocatore* g){
    if(g == NULL){
        return;
    }

    Tipo_oggetto oggetto = NESSUN_OGGETTO;

    if(g->mondo == 0){
        oggetto = g->pos_mondoreale->oggetto;
    } else{
        printf("Sei nel Sopra Sotto non ci sono oggetti da prendere \n");
        return;
    }

    //controllo oggetto
    if(oggetto == NESSUN_OGGETTO){
        printf("Non c'e ninente da raccogliere. \n");
        return;
    }
    //controllo il nemico
    if(zona_ha_nemico(g)){
        printf("devi prima soconfiggere il nemico \n");
        return;
    }

    for(int i=0; i<3; i++){
        if(g->zaino[i] == NESSUN_OGGETTO){
            g->zaino[i] = oggetto;
            goto rimuovi;  //skippo il reseto del codice setrovo un posto livero
        }
    }


    printf("Zaino pieno!\n");
    for(int i=0; i<3; i++){
        printf("%d Oggetto %s\n", i+1, nome_oggetto(g->zaino[i]));
    }
    printf("scegli quele oggetto buttare (tra 1 e 3) o 0 per annullare:");
    int scelta = leggi_numero();
    printf("\n");

    if(scelta < 1 || scelta > 3){
        printf("Operazione annullata. \n");
        return;
    }


    //scambio
    Tipo_oggetto scarto = g->zaino[scelta-1];
    g->zaino[scelta-1] = oggetto;

    if(g->mondo == 0){
        g->pos_mondoreale->oggetto = scarto;
    }

    printf("Hai scambio l'oggetto. \n");
    return;

    rimuovi:{
        if(g->mondo==0){
            g->pos_mondoreale->oggetto = NESSUN_OGGETTO ;
        }
        printf("Oggetto raccolto \n");
    }
}


static void utilizza_oggetto(Giocatore*g, int in_combat, int* vita_nemico){
    if(!g){
        return;
    }

    int oggetto_utilizabile = 0;
    printf("\n ---Zaino --- \n");
    for(int i=0; i< 3; i++){
        if(g->zaino[i] != NESSUN_OGGETTO){
            printf("%d Oggetto %s \n", i+1, nome_oggetto(g->zaino[i]));
            descrivi_oggetto(g->zaino[i]);
            oggetto_utilizabile =1;
        }
    }

    if(!oggetto_utilizabile){
        printf("Zaino vuoto. \n");
        return;
    }

    printf("Sceglio l'oggetto da utilizare tra 1-3 o 0 per annulare\n");

    int scelta=leggi_numero();
    if(scelta<1 || scelta>3){
        return;
    }

    Tipo_oggetto oggetto = g->zaino[scelta-1];
    if(oggetto == NESSUN_OGGETTO){
        printf("Slot vuoto. \n");
        return;
    }

    switch(oggetto){
        case BICICLETTA:
            if(in_combat){
                printf("Non puoi usare la bicicletta nel mentre che combatti\n");
            }else{
                printf("%s usa la bicicletta: poi muoverti di nuovo\n", g->nome);
                g->movimento_extra_turni =1 ;
            }

            break;

        case MAGLIETTA_FUOCOINFERNO:
            printf("Maglietta infuocata: +21 di difesa \n");
            g->difesa_psichica = g->difesa_psichica + 21;
            break;

        case BUSSOLA:
            printf("Usi la bussola: \n");
            usa_bussola(g);
            break;


        case CHITARRA_METALLICA:
            if(!zona_ha_nemico(g)){
                printf("Non ce nessun nemico");
                return;
            }
            printf("suoini la chitarra metallica: 30 danni al nemico");
            *vita_nemico = * vita_nemico - 30;
            break;

        default:
            printf("Slot vuot \n");
            return;

    }

    //tutti gli oggetti singolo utilizzo
    g->zaino[scelta-1] = NESSUN_OGGETTO;
}






/* ================= COMBATTIMENTO ================= */

static void reset_stat_giocatore(Giocatore* g){
    if(!g){
        return;
    }

    g->attacco_psichico = g->att_base;
    g->difesa_psichica = g->dif_base;
    g->fortuna = g->fort_base;
}

static Esito_combattimento combatti(Giocatore* g){

    //controllo in caso se ci sia qalcosa 
    if(!zona_ha_nemico(g)){
        printf("Non c'è nessun nemico da combattere.\n");
        return NIENTE;
    }

    //creazione stats nemico in base a chi sia 
    Tipo_nemico tipo;
    char nome_nemico[30];
    int nemico_vita=0;
    int nemico_attacco=0;
    int nemico_difesa=0;

    if(g->mondo == 0){
        tipo = g->pos_mondoreale->nemico;
    }
    else{
        tipo = g->pos_soprasotto->nemico;
    }

    //tipi di nemici
    switch(tipo){
        case DEMOCANE:
            strcpy(nome_nemico, "DEMOCANE");
            nemico_vita=50; 
            nemico_attacco=14; 
            nemico_difesa=8;  
            break;

        case BILLI:
            strcpy(nome_nemico, "BILLI");
            nemico_vita=30; 
            nemico_attacco=12; 
            nemico_difesa=5; 
            break;

        case DEMOTORZONE:
            strcpy(nome_nemico, "DEMOTORZONE");
            nemico_vita=100; 
            nemico_attacco=17; 
            nemico_difesa=12; 
            break;

        default: 
            return NIENTE;
    }

    printf("\n -----Inizio Combattimeto contro %s------ \n", nome_nemico);

    while(g->hp > 0 && nemico_vita > 0){
        printf("[ Stato: %s HP: %d | %s HP: %d ]\n", g->nome, g->hp, nome_nemico, nemico_vita);
        int scelta;
        do{
            printf("1) Attacca\n2) Usa oggetto\n3) Fuggi\n");
            scelta = leggi_numero();
        }while(scelta < 1 || scelta > 3);

        //turno del giocatore 
        if(scelta == 1){
            int tiro = g->attacco_psichico + lancio_dado(20);
            int danno = tiro - nemico_difesa;
            if(danno < 1){
                danno = 0;
            }
            nemico_vita -= danno;
            printf("\n>>> ATTACCHI: Hai tirato %d (Totale: %d). Fai %d danni a %s!\n", tiro, danno , danno, nome_nemico);
        }
        else if(scelta == 2){
            utilizza_oggetto(g, 1, &nemico_vita);
        }
        else if(scelta == 3){
            if(tipo == DEMOTORZONE){
                printf("Non puoi scappare dal DEMOTORZONE!\n");
            }else{
                int dado = lancio_dado(20);
                if(dado < g->fortuna){
                    return FUGA;
                }
            }
            printf("Fuga fallita.\n");
            
        }

        //turno nemico
        if(nemico_vita > 0){
            int tiro = nemico_attacco + lancio_dado(20);
            int danno = tiro - g->difesa_psichica;
            if(danno < 1){
                danno = 0;
            }
            g->hp -= danno;
            printf("<<< %s ATTACCA: Tira %d (Totale: %d). Subisci %d danni! (I tuoi HP: %d)\n", nome_nemico, tiro, danno, danno, g->hp);
        }

        //controllo se il giocatore e morto
        if(g->hp <= 0){
            g->vivo = 0;
            printf("%s SEI MORTO", g->nome);
            reset_stat_giocatore(g);
            g->hp = 0;
            return MORTE;
        }


        if(nemico_vita <= 0){
            if(tipo != DEMOTORZONE){
               if(g->mondo == 0){

                g->pos_mondoreale->nemico = NESSUN_NEMICO;
                }else{
                g->pos_soprasotto->nemico = NESSUN_NEMICO;
                }
                reset_stat_giocatore(g);
                printf("\n************************************\n");
                printf("   VITTORIA! %s e' stato sconfitto!   \n", nome_nemico);
                printf("************************************\n");
                return VITTORIA;

            }else{
                printf("Congratulazioni %s, hai sconfitto il Demotorzone!\n", g->nome);
                reset_stat_giocatore(g);
                return UCCISO;
            }
        }
    }

    if(g->hp < 75){
        int recupero_hp = (75 * 20 ) / 100; //20% percento dei hp;
        g->hp = g->hp + recupero_hp;
        if(g->hp > 75){
            g->hp = 75;
        }
        printf("dopo il combattimento hai recuperato il 20 perchento della vita totale \n");
    }
    printf("La ta vita è al massimo non puoi curarti oltre");
    reset_stat_giocatore(g);
    return VITTORIA;
}


/* ================= PASSA================= */


    static Esito_combattimento passa(Giocatore* g){
        printf("%s passa il truno al prossimo giocatore \n", g->nome);
        return NIENTE;
    }


/* ================= MENU GIOCATORE  ================= */

static Esito_combattimento turno_giocatore(Giocatore* g){
    int scelta=0;
    int azioni = 0;


    while(1){


        do{
            printf("\n Turno di : %s \n ", g -> nome );
            printf("1) Avanza\n");
            printf("2) Inidetreggia\n");
            printf("3) Cambio Mondo\n");
            printf("4) Combatti\n");
            printf("5) Raccogli oggetto \n");
            printf("6) Utilizza oggetto \n");
            printf("7) Stampa Giocatore \n");
            printf("8) Stampa Zona\n");
            printf("9) Passa\n");
            printf("Scelta : ");
            scelta= leggi_numero();
            if(scelta < 1|| scelta >9 ){
                printf("Valore non valido riporova");
            }
        }while(scelta < 1|| scelta >9 );


        switch(scelta){
            case 1:
                if(azioni){
                    printf("Hai gia avanzato .\n");
                    break;
                } 
                avanza(g);
                azioni = 1;
                if(g->movimento_extra_turni){
                    g->movimento_extra_turni = 0;
                    azioni = 0;
                    printf("Grazie alla bicicletta puoi muoverti di nuovo \n");
                }else{
                    azioni =1;
                }
                break;

            case 2:
                if(azioni){
                    printf("Hai gia avanzato .\n");
                    break;
                } 
                indietreggia(g);
                azioni = 1;
                if( g->movimento_extra_turni ){
                    g->movimento_extra_turni = 0;
                    azioni = 0;
                    printf("Grazie alla bicicletta puoi muoverti di nuovo \n");
                }else{
                    azioni = 1;
                }

                break;

            case 3:
                if(azioni){
                    printf("Hai gia avanzato .\n");
                    break;
                } 
                cambia_mondo(g, &azioni);
                azioni = 1;
                if(g->movimento_extra_turni){
                    g->movimento_extra_turni = 0;
                    azioni = 0;
                    printf("Grazie alla bicicletta puoi muoverti di nuovo \n");
                }else{
                    azioni=1;
                }
                break;
            case 4:{
                Esito_combattimento esito = combatti(g);
                if(esito == MORTE || esito  == UCCISO){
                    return esito;
                }
                break;
            }


            case 5:{
                raccogli_oggetto(g);
                break;
            }

            case 6:{
                utilizza_oggetto(g, 0, 0);
                break;
            }

            case 7:{
                stampa_giocatore(g); // attento qua ho gia una funzione chiamata cosi;
                break;
            }

            case 8:{
                stampa_zona_giocatore(g); // attento qua ho gia una funzione chiamata cosi;
                break;
            }

            case 9:{
                return passa(g);
            }

            default:
            printf("scelta non valida, riprova\n");
            break;
        }
    }
}

/* =================GIOCA ================= */

void gioca(){
    if(!mappa_chiusa){
        printf("Errore: la mappa non e stata chiusa! \n");
        return;
    }

    //inizio del gioco 

    printf("=== INIZIO GIOCO ===");
    inizializza_giocatori();
    int ordine[numero_giocatori];
    
    while(!gioco_finito){
        pulischi_morti();

        if(tutti_giocatori_morti()){
            printf("\n==========================================\n");
            printf("  GAME OVER: tutti i giocaotri sono morti \n");
            printf("==========================================\n");
            gioco_finito=1;
            break; //esco dal while
        }

        genera_ordine_casuale(ordine);
        

        //turni
        for(int i=0; i<numero_giocatori; i++){
            //1 genero ordine casuale per questo turno
            Giocatore* g = giocatori[ordine[i]];

            //Salta se il giocatore è morto
            if( g == NULL  || !g-> vivo){
                continue;
            }
            
            printf("\n--- Turno di %s ---\n", g->nome);
            Esito_combattimento esito = turno_giocatore(g);
            
            if(esito == UCCISO){
                gioco_finito =1;
                break;
            }

            if(esito == MORTE){
                printf("%s è morto durante il suo truno \n", g->nome);
                g->vivo =0;

                if(tutti_giocatori_morti()){
                    gioco_finito = 1;
                    break;
                }
            }
            

            if(esito == FUGA){
                printf("%s è riuscito a scappare \n", g->nome);
            }
        }
    }
    printf("\n=== FINE DEL GIOCO ===\n");
}

/* =================TERMINE GIOCO ================= */


static void aggiorna_vincitori(const char* vincitore){
    for(int i =MAX_VINCITORI-1 ; i>0; i--){
        strcpy(ultimi_vincitori[i], ultimi_vincitori[i-1]);      
    }   

    strncpy(ultimi_vincitori[0], vincitore, 49);
    ultimi_vincitori[0][49]='\0';
}


void termina_gioco(Giocatore* vincitore){

    if(numero_giocatori> 0){
        if(vincitore != NULL){
             printf("\n====================================================\n");
            printf("congratulazioni %s, hai vinto!\n", vincitore->nome_originale);
            printf("\n====================================================\n");
            aggiorna_vincitori(vincitore->nome_originale);
        }
    }else{
        printf("partita interrotta o nessun vincitore. morti ?\n ");
    }
    pulitore_mappa();

    libera_giocatori();

    gioco_finito=0;
    mappa_chiusa=0;
    numero_giocatori=0;
    printf("\n sistema restto correttamente\n");
    printf("\n GRAZIE PER AVER GIOCATO \n");
}


/* =================CREDITI ================= */

void crediti() {
    printf("\n======================CREDITI======================\n");
    printf("Creatore del gioco: Daniel\n\n");
    printf("Numero di matricola: 390477"); 
    printf("\n====================================================\n");

    printf("Ultimi vincitori: \n");
    for(int i=0; i<MAX_VINCITORI; i++){
        printf("%d) %s\n", i+1, ultimi_vincitori[i]);
    }
}