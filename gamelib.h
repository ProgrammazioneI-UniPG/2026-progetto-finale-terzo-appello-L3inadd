void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();
int leggi_numero();

typedef enum{ 
    NESSUN_OGGETTO,
    BICICLETTA,
    MAGLIETTA_FUOCOINFERNO,
    BUSSOLA,
    CHITARRA_METALLICA
    }Tipo_oggetto;

typedef enum{
    BOSCO,
    SCUOLA,
    LABORATORIO,
    CAVERNA,
    STRADA,
    GIARDINO,
    SUPERMERCATO,
    CENTRALE_ELETTRICA,
    DEPOSITO_ABBANDONATO,
    STAZIONE_POLIZIA
}Tipo_zona;

typedef enum{
    NESSUN_NEMICO,
    DEMOCANE,
    BILLI,
    DEMOTORZONE
}Tipo_nemico;

typedef enum {
    NIENTE,             // nessun nemico presente
    VITTORIA,           // nemico sconfitto
    FUGA,               // giocatore è fuggito
    MORTE,
    UCCISO      // il boss è stato sconfitto
}Esito_combattimento; 

typedef struct Giocatore {
    char nome [35];
    char nome_originale[35];
    int vivo;
    int hp;
    int speciale;
    int mondo;
    struct Zona_mondoReale* pos_mondoreale; //puntatore alla struct mondo reale 0
    struct Zona_sopraSotto* pos_soprasotto; // '' dipendono da mondo  1
    int attacco_psichico;
    int difesa_psichica;  // da 0 a 20 
    int fortuna;
    Tipo_oggetto zaino [3];
    
    //salvataggio stast per possibili modifiche 
    int att_base;
    int dif_base;
    int fort_base;

    //per gli effetti degli oggetti (item)
    int movimento_extra_turni;
}Giocatore;





typedef struct Zona_mondoReale{
    Tipo_zona  zona;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto; 
    struct Zona_mondoReale* avanti;
    struct Zona_mondoReale* indietro;
    struct Zona_sopraSotto* link_sopraSotto;
} Zona_mondoReale;

typedef struct Zona_sopraSotto{
    Tipo_zona zona;
    Tipo_nemico nemico;
    struct Zona_sopraSotto* avanti;
    struct Zona_sopraSotto*indietro;
    struct Zona_mondoReale* link_mondoReale;    
} Zona_sopraSotto;

