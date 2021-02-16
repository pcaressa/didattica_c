/** \file bsgen.c

    \brief
    Bullshit Generator: toy version del classico generatore di stronzate markoviano.
    
    \note
    Questo programma didattico:
        1. Legge uno o più file di testo
        2. Individua tutte le parole
        3. Inserisce ciascuna parola in un dizionario di coppie (chiave, valore)
            in cui la chiave e' la parola e il valore la lista di tutte le parole
            che la seguono da qualche parte nel testo.
        4. Sceglie parole a caso che iniziano con la maiuscola e genera a caso
            una delle parole che la seguono: poi ripete a partiere da questa
            fino a trovare una parola che termini con un punto e stampa la frase.

    \todo
    Esercizi:
    - Sostituire la ricerca lineare nella funzione inserisci_parola con una
        ricerca binaria, avendo cura di, quando si inserisce una nuova parola,
        mantenere l'ordinamento alfabetico delle parole. Usare bsearch per
        trovare una parola invece del ciclo for iniziale.
    - Al momento ogni nuova parola viene allocata: questo spreca un sacco di
        spazio, modificare il programma in modo da memorizzare tutte le parole
        in una lista e usare i loro indirizzi per inserirle sia nel campo key
        che nel campo value[i] di un elemento coppia_t.
    - Sostituire la ricerca lineare con una ricerca Hash: per farlo occorre
        definire una funzione hash (semplice) che fornisca l'indice di una
        parola inserire tutte le parole in una singola tavola hash (mentre
        ora vengono allocate con gran spreco di memoria).
    - Dare la possibilità di salvare su file il testo generato.
*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **value;
    int len;
} coppia_t;

typedef struct {
    coppia_t *p;
    int len;
} tabella_t;

void stampa_tabella(tabella_t *tabella)
{
    printf("tabella.len = %i\ntabella.p = { ", tabella->len);
    for (int i = 0; i < tabella->len; ++ i) {
        printf("%s: [", tabella->p[i].key);
        for (int j = 0; j < tabella->p[i].len; ++ j) {
            printf(" %s", tabella->p[i].value[j]);
        }
        printf("] ");
    }
    printf("}\n");
}

/** Inserisce una nuova parola nel dizionario oppure torna l'indirizzo della parola
    se questa già esiste. */
coppia_t *inserisci_parola(tabella_t *tabella, const char *parola)
{
//printf("inserisci_parola(%p, %s)", tabella, parola);
    for (int j = 0; j < tabella->len; ++ j) {
        if (strcmp(tabella->p[j].key, parola) == 0) {
            return tabella->p + j;
        }
    }
    tabella->p = realloc(tabella->p, (tabella->len+1)*sizeof(coppia_t));
    assert(tabella->p != NULL);
    tabella->p[tabella->len].key = malloc(strlen(parola) + 1);
    assert(tabella->p[tabella->len].key != NULL);
    strcpy(tabella->p[tabella->len].key, parola);
    tabella->p[tabella->len].value = NULL;
    tabella->p[tabella->len].len = 0;
    return tabella->p + tabella->len++;
}

/** Aggiunge all'array value della coppia un nuovo elemento dato da s. */
void aggiungi(coppia_t *coppia, const char *s)
{
//printf("aggiungi(%p, %s)", coppia, s);
    coppia->value = realloc(coppia->value, (coppia->len + 1)*sizeof(char*));
    assert(coppia->value != NULL);
    coppia->value[coppia->len] = malloc(strlen(s) + 1);
    assert(coppia->value[coppia->len] != NULL);
    strcpy(coppia->value[coppia->len], s);
    ++ coppia->len;
}

/** Apre il file, scandice le parole e crea il dizionario, inserendo per ciascuna parola
    quella che la seguono nei valori associati alla sua chiave. */
void leggi(const char *nome_file, tabella_t *tabella)
{
    char buffer[128];
    FILE *file = fopen(nome_file, "r");
    assert(file != NULL);
    if (fscanf(file, "%s", buffer) == 1) {
        for (;;) {
            coppia_t *p = inserisci_parola(tabella, buffer);
            if (fscanf(file, "%s", buffer) != 1) {
                break;
            }
            aggiungi(p, buffer);
        }
    }
    fclose(file);
}

/** Sceglie una parola a caso e la usa per generare un testo che stampa. */
void genera(tabella_t *tabella)
{
    /** Sceglie la prima parola con la maiuscola. */
    int i;
    do {
        i = ((double)rand() / RAND_MAX) * tabella->len;
    } while (islower(tabella->p[i].key[0]));

    printf("%s ", tabella->p[i].key);
    for (;;) {
        int j = ((double)rand() / RAND_MAX) * tabella->p[i].len;
        printf("%s ", tabella->p[i].value[j]);
        /** Termina se la parola contiene un punto. */
        if (strchr(tabella->p[i].value[j], '.') != NULL) {
            break;
        }
        coppia_t *p = inserisci_parola(tabella, tabella->p[i].value[j]);
        i = p - tabella->p;
    }
}

int main(int argc, char**argv)
{
    tabella_t tabella;
    tabella.p = NULL;
    tabella.len = 0;

    //stampa_tabella(&tabella);
    printf("Scandisco i file...");
    int i;
    for (i = 1; i < argc; ++ i) {
        leggi(argv[i], &tabella);
    }
    //stampa_tabella(&tabella);    
    if (i > 0) {
        char buffer[16];
        printf("...Batti invio per generare frasi, premi CTRL-C per terminare.\n");
        while(fgets(buffer, sizeof(buffer), stdin) != NULL)
            genera(&tabella);
    }
    return 0;
}
