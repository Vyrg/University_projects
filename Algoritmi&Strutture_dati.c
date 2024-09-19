#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define TABLE_SIZE 65521
//#define TABLE_SIZE 997
#define MAX_COMMAND 3
#define MAX_NOME 20

typedef struct Lotto {
    int peso;
    int scadenza;
    struct Lotto * next;
} Lotto;

typedef struct Scaffale {
    char nome[MAX_NOME];
    int tot;
    Lotto * head;
} Scaffale;

typedef struct ingrediente{    
    Scaffale *scaffale;
    int qnt;
    struct ingrediente *next;
} ingrediente;

typedef struct Ricetta {    
    char nome[MAX_NOME];
    int peso_tot;
    int provato;
    int tempo_prova;
    ingrediente *head;
    ingrediente *tail;
} Ricetta;

typedef struct Ordine {
    Ricetta *ric;
    int arrivo;
    int qnt;
    int peso_tot;
    struct Ordine *next;
    } Ordine;

/************************************************************************************************/
//DICHIARAZIONE STRUTTURE DATI
Ricetta *Ricettario[TABLE_SIZE] = {NULL};
Ricetta *TOMBSTONE = (Ricetta *) -1;

Scaffale *Magazzino[TABLE_SIZE] = {NULL};
//Scaffale *TOMBSTONE_MAG = (Scaffale *) -1;

/************************************************************************************************/
// ***DICHIARAZIONE FUNZIONI PER RICETTARIO***/

unsigned int Hash(char* str) {
    if(str != NULL) {
        int hash = 0;
        int i = 0;

        while(str[i] != '\0'){
            hash += (int) str[i];
            i++;
        }
        return hash % TABLE_SIZE;
    } else return 0;
}

unsigned int Hash2(char *str){
   if(str != NULL) {
        int hash = 0;
        int i = 0;

        while(str[i] != '\0'){
            hash += (int) str[i];
            i++;
        }
        return hash % TABLE_SIZE;
    } else return 1;
}

Scaffale *creaScaffale(char *nome, Lotto *lot);

Scaffale* findScaf(Scaffale ** facchino, char *nome){
    int i = 0;
    int h1 = Hash(nome);
    int h2 = Hash2(nome);
    int point;

    while(i < TABLE_SIZE){
        point = (h1 + i*h2) % TABLE_SIZE;

        if(facchino[point] == NULL){
            facchino[point] = creaScaffale(nome, NULL);
            return facchino[point];
        }
        if((facchino[point] != NULL)){
           if(strncmp(facchino[point]->nome, nome, MAX_NOME) == 0){
            return facchino[point];
            } 
        }

        i++;
    }

    return NULL;
}

//nodo per lista ingredienti
ingrediente *nuovo_ingrediente(char *nome, int qnt){
    ingrediente *newIng = NULL;
    newIng = (ingrediente *) malloc(sizeof(ingrediente));
    newIng->scaffale = findScaf(Magazzino, nome);
    newIng->qnt = qnt;
    newIng->next = NULL;
    return newIng;
}

//aggiungere un ingrediente ad una ricetta
void inserisciIngrediente(Ricetta* ric, char* nome, int qnt){
    ingrediente *newIng = nuovo_ingrediente(nome, qnt);
    if(ric->head == NULL){
        ric->head = newIng;
        ric->tail = newIng;
        return;
    } else {
        ric->tail->next = newIng;
        ric->tail = newIng;
        return;
    }
    return;
}

Ricetta* nuova_ricetta(char* n){
    Ricetta* newRic = NULL;
    newRic = (Ricetta *) malloc(sizeof(Ricetta));
    
    strcpy(newRic->nome, n);
    newRic->peso_tot = 0;
    newRic->provato = 0;
    newRic->tempo_prova = 0;
    newRic->head = NULL;
    newRic->tail = NULL;
    return newRic;
}

//aggiungere ricetta a Ricettario
void inserisciRicetta(Ricetta ** indice, Ricetta* ric){
    int i = 0;
    int h1 = Hash(ric->nome);
    int h2 = Hash2(ric->nome);

    while(indice[(h1 + i*h2) % TABLE_SIZE] != NULL){
        i++;
    }
    indice[(h1 + i*h2) % TABLE_SIZE] = ric;
}


//cerco ricetta in ricettario
int findRicetta(Ricetta** ind, char* n){
    int i = 0;
    int h1 = Hash(n);
    int h2 = Hash2(n);


    while(ind[(h1 + i*h2) % TABLE_SIZE] != NULL){
        int point = (h1 + i*h2) % TABLE_SIZE;
        if((ind[point] != NULL && ind[point] != TOMBSTONE) && ind[point]->nome != NULL){
           if(strncmp(ind[point]->nome, n, MAX_NOME) == 0){
            return point;
            } 
        }
        if(i == TABLE_SIZE) return 0x0;
        i++;
    }
    return 0x0;
}

void rmIngredienti(Ricetta *ric){
    ingrediente *curr = ric->head;
    ingrediente *tmp = NULL;
    while(curr != NULL){
        tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    ric->head = NULL;
    ric->tail = NULL;
}

bool rmRicetta(Ricetta **indice, int i){
    Ricetta * ric = indice[i]; 

    if(ric != NULL && ric != TOMBSTONE){
        rmIngredienti(ric);
        free(ric);
        indice[i] = TOMBSTONE;
        return true;
    } else return false;
}
/**************************************************************/
//***DICHIARAZIONE FUNZIONI PER ORDINI***

Ordine *creaOrdine(int arr, Ricetta *ric, int qnt){
    Ordine *newOrd = (Ordine *)malloc(sizeof(Ordine));

    newOrd->arrivo = arr;
    newOrd->qnt = qnt;
    newOrd->peso_tot = (ric->peso_tot) * qnt;
    newOrd->ric = ric;
    newOrd->next = NULL;

    return newOrd;
}

void insertOrdine(Ordine **head, Ordine **tail, Ordine* newOrd){
    //caso di lista vuota
    if(*head == NULL){
        *head = newOrd;
        *tail = newOrd;
        return;
    }

    //in coda
    (*tail)->next = newOrd;
    *tail = newOrd;
    return;
}

bool checkOrdine(Ordine *head, Ricetta *ric){
    Ordine *curr=head;

    while(curr != NULL){
        if(curr->ric == ric){
            return true;
        }
        curr = curr->next;
    }
    return false;
}

int confrontaOrdini(const void *a, const void *b){
    Ordine *ordA = (Ordine *)a;
    Ordine *ordB = (Ordine *)b;

    if(ordA->peso_tot < ordB->peso_tot){
        return 1;
    } else if(ordA->peso_tot > ordB->peso_tot){
        return -1;
    } else {
        if(ordA->arrivo > ordB->arrivo){
            return 1;
        } else if(ordA->arrivo < ordB->arrivo){
            return -1;
        } else {
            return 0;
        }
    }
}

void stampaOrdini(Ordine *caricati, int k){
    Ordine *daStampa = (Ordine *)malloc(k*sizeof(Ordine));
    Ordine *tmp = caricati;
    for(int i=0; i<k; i++){
        daStampa[i] = *tmp;
        tmp = tmp->next;
    }

    qsort(daStampa, k, sizeof(Ordine), confrontaOrdini);

    for(int i=0; i<k; i++){
        printf("%d %s %d\n", (daStampa[i]).arrivo, (daStampa[i].ric)->nome, daStampa[i].qnt);
    }

    free(daStampa);
}
//cerca mergesort su cormen
void dividiLista(Ordine * head, Ordine **front, Ordine **back){
    Ordine *fast;
    Ordine *slow;
    slow = head;
    fast = head->next;

    while(fast != NULL){
        fast = fast->next;
        if(fast != NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = head;
    *back = slow->next;
    slow->next = NULL;
}

Ordine *merge(Ordine *a, Ordine *b){
    Ordine *result = NULL;

    if(a == NULL){
        return b;
    } else if(b == NULL){
        return a;
    }

    if(a->arrivo <= b->arrivo){
        result = a;
        result->next = merge(a->next, b);
    } else {
        result = b;
        result->next = merge(a, b->next);
    }
    return result;
}

void mergeSort(Ordine **head){
    Ordine *tmp = *head;
    Ordine *a;
    Ordine *b;

    if(tmp == NULL || tmp->next == NULL){
        return;
    }

    dividiLista(tmp, &a, &b);

    mergeSort(&a);
    mergeSort(&b);

    *head = merge(a, b);
}

//*************************************************************** */
//***DICHIARAZIONE FUNZIONI PER MAGAZZINO***

Lotto* creaLotto(int peso, int scadenza) {
    Lotto *newNode = (Lotto *)malloc(sizeof(Lotto));
    newNode->peso = peso;
    newNode->scadenza = scadenza;
    newNode->next = NULL;
    return newNode;
}

Scaffale *creaScaffale(char *nome, Lotto *lot){
    Scaffale *newScaf = (Scaffale *)malloc(sizeof(Scaffale));
    strcpy(newScaf->nome, nome);
    newScaf->tot = 0;
    newScaf->head = lot;
    return newScaf;
}

//ordina scaffale per scadenza
Lotto * ordina_scaffale(Lotto **head, Lotto* new){
    //in testa
    if(*head == NULL || (*head)->scadenza > new->scadenza) { 
        new->next = *head;
        return new;
    }

    if((*head)->scadenza == new->scadenza) {
        (*head)->peso += new->peso;
        return *head;
    }

    //scorro la lista 
    Lotto *curr = *head;
    if(curr->next != NULL){
        while(curr->next != NULL && curr->next->scadenza <= new->scadenza){
            if(curr->next->scadenza == new->scadenza){
                curr->next->peso += new->peso;
                return *head;
            }
            curr = curr->next;
        }
    }
    
    new->next = curr->next;
    curr->next = new;
    return *head;
}

//inserisce lotto in magazzino
void inserisciLotto(Scaffale ** facchino, Lotto* lot, char *nome) {
    int i = 0;
    int h1 = Hash(nome);
    int h2 = Hash2(nome);

    int index = h1;
    while(facchino[index] != NULL){
        if(strncmp(facchino[index]->nome, nome, MAX_NOME) == 0) {
            facchino[index]->tot += lot->peso;
            facchino[index]->head = ordina_scaffale(&(facchino[index]->head), lot);
            return;
        } else {
            i++;
        }
        index = (h1 + i*h2) % TABLE_SIZE;
    }
    facchino[index] = creaScaffale(nome, lot);
    facchino[index]->tot = lot->peso;
}

//controllo disponibilità
bool checkMag(Ricetta *ric, int qnt, int t){
    if(ric == NULL || ric == TOMBSTONE){
        return false;
    }

    if(ric->tempo_prova == t && qnt >= ric->provato){
        return false;
    }

    ingrediente *ing = ric->head;
    Scaffale *scaf = ing->scaffale;
    //rimozione lotti scaduti
    if(scaf != NULL){
        Lotto *curr = scaf->head;

        while(curr != NULL && curr->scadenza <= t){
            scaf->tot -= curr->peso;
            curr = curr->next;
        }
        scaf->head = curr;
    }

    int necessario = ing->qnt*qnt;
    while(ing != NULL && scaf != NULL){
        if(scaf->tot < necessario){
            ric->tempo_prova = t;
            ric->provato = qnt;
            
            return false;
        }
        ing = ing->next;
        if(ing != NULL){
            scaf = ing->scaffale;
            //rimozione lotti scaduti
            if(scaf != NULL){
                Lotto *curr = scaf->head;

                while(curr != NULL && curr->scadenza <= t){
                    scaf->tot -= curr->peso;
                    curr = curr->next;
                }
                scaf->head = curr;
            }

            necessario = ing->qnt*qnt;
        }
    }
    return true;
}

void consumaLotti(Scaffale **facchino, Ricetta *ric, int qnt, int t){
    ingrediente *ing = ric->head;
    Scaffale *scaf = ing->scaffale;
    //rimozione lotti scaduti
    if(scaf != NULL){
        Lotto *curr = scaf->head;

        while(curr != NULL && curr->scadenza <= t){
            scaf->tot -= curr->peso;
            curr = curr->next;
        }
        scaf->head = curr;
    }

    int consumo = 0;
    Lotto *curr = NULL;

    while(ing != NULL){
        consumo = ing->qnt*qnt;
        curr = scaf->head;
        while(curr != NULL && consumo > 0){
            if(curr->peso > consumo){
                curr->peso -= consumo;
                scaf->tot -= consumo;
                consumo = 0;
            } else if(curr->peso <= consumo){
                consumo -= curr->peso;
                scaf->tot -= curr->peso;
                scaf->head = curr->next;
                free(curr);
                curr = scaf->head;
            }
        }
        ing = ing->next;
        if(ing != NULL){
            scaf = ing->scaffale;
            //rimozione lotti scaduti
            if(scaf != NULL){
                Lotto *curr = scaf->head;

                while(curr != NULL && curr->scadenza <= t){
                    scaf->tot -= curr->peso;
                    curr = curr->next;
                }
                scaf->head = curr;
            }
        }
    }
}   


/**********************************************************************/


int main(int argc, char *argv[]){

    int useless = 0;
    char controllo = 'a';

    //variabili camioncino
    int periodo;
    int capienza;
    int carico = 0;
    int flag = 0;
    int tempo = 0;
    char comando[17];
    //variabili Magazzino
    char nome_lot[MAX_NOME];
    int peso = 0;
    int scadenza = 0;
    Scaffale **facchino = Magazzino;
    //variabili ordini
    int qnt_ord = 0;
    int ord_caricati = 0;
    //variabili ricettario
    char nome_ric[MAX_NOME];
    char nome_ing[MAX_NOME];
    int qnt = 0;

    //inizializzo ricettario
    Ricetta **indice = Ricettario;

    //inizializzo lista ordini
    Ordine *listaReady = NULL;
    Ordine *codaReady = NULL;

    Ordine *listaAttesa = NULL;
    Ordine *codaAttesa = NULL;

    useless = scanf("%d %d ", &periodo, &capienza);
    useless = scanf("%s", comando);

    while(useless != EOF){
        flag = 0;
        if(tempo != 0 && periodo != 1 && tempo % periodo == 0){
            if(listaReady == NULL){
                printf("camioncino vuoto\n");
            } else {
/**************************************************************************/
                ord_caricati = 0;
                carico = 0;

                mergeSort(&listaReady);

                //conto quanti ordini entrano nel camioncino
                Ordine *sentinella = listaReady;
                while(sentinella != NULL){
                    if(sentinella != NULL){
                        if(sentinella->peso_tot + carico < capienza){
                        carico += sentinella->peso_tot;
                        ord_caricati++;
                        sentinella = sentinella->next;
                        } else break;                               
                    }
                }
                //funzione per stampare i primi ord_caricati in ordine di peso
                stampaOrdini(listaReady, ord_caricati);   
                for(int j=0; j<ord_caricati; j++){
                    if(listaReady != NULL){
                        listaReady = listaReady->next;
                    }
                }
                //aggiusta la coda della lista
                sentinella = listaReady;
                while(sentinella != NULL && sentinella->next != NULL){
                    sentinella = sentinella->next;
                }
                codaReady = sentinella;
/***********************************************************************/
            }
            
        }
        //***Comando aggiungi_ricetta***
        if(strncmp(comando, "aggiungi_ricetta", MAX_COMMAND) == 0){
            flag = 0;
            useless = scanf("%c", &controllo);
            useless = scanf("%s", nome_ric);
            //controllo se già esiste
            int index = findRicetta(indice, nome_ric);
            if(indice[index] != NULL && indice[index] != TOMBSTONE){
                flag = 1;
            }
            if(flag == 1){
                useless = scanf("%c", &controllo);
                while(controllo != '\n'){
                    useless = scanf("%s ", nome_ing);
                    useless = scanf("%d", &qnt);
                    useless = scanf("%c", &controllo);
                }
                printf("ignorato\n");
            } else { 
                //aggiungo ricetta con ingredienti
                Ricetta *new_ric = nuova_ricetta(nome_ric);
                useless = scanf("%c", &controllo);
                while(controllo != '\n'){
                    useless = scanf("%s ", nome_ing);
                    useless = scanf("%d", &qnt);
                    new_ric->peso_tot += qnt;
                    inserisciIngrediente(new_ric, nome_ing, qnt);
                    useless = scanf("%c", &controllo);
                }
                inserisciRicetta(indice, new_ric);
                printf("aggiunta\n");
            }
        //***Comando rimuovi_ricetta***
        } else if(strncmp(comando, "rimuovi_ricetta", MAX_COMMAND) == 0){
            flag = 0;
            useless = scanf("%c", &controllo);
            useless = scanf("%s ", nome_ric);
            int index = findRicetta(indice, nome_ric);
            Ricetta *ric = indice[index];
            //controllo se esiste
            if(ric != NULL && ric != TOMBSTONE){
                flag = 1;
            }
            if(flag == 0){
                printf("non presente\n");
            } else if(flag == 1){
                if(checkOrdine(listaReady, ric) || checkOrdine(listaAttesa, ric)){
                    printf("ordini in sospeso\n");
                } else {
                    rmRicetta(indice, index);
                    printf("rimossa\n");
                }
            }
        //***Comando ordine***
        } else if(strncmp(comando, "ordine", MAX_COMMAND) == 0){
            flag = 0;
            //controllo se esiste la ricetta
            useless = scanf("%c", &controllo);
            useless = scanf("%s", nome_ric);
            int index = findRicetta(indice, nome_ric);
            Ricetta *ric = indice[index];

            if(ric != NULL && ric != TOMBSTONE){
                flag = 1;
            }
            
            if(flag == 1){
                useless = scanf("%d", &qnt_ord);
                Ordine* newOrd = creaOrdine(tempo, ric, qnt_ord);
                
                printf("accettato\n");

                //controllo se ho abbastanza lotti in Magazzino
                if(checkMag(ric, qnt_ord, tempo)){
                    insertOrdine(&listaReady, &codaReady, newOrd);
                    consumaLotti(Magazzino, ric, qnt_ord, tempo);

                } else {
                    insertOrdine(&listaAttesa, &codaAttesa, newOrd);
                }
            } else {
                useless = scanf("%d", &qnt_ord);
                printf("rifiutato\n");
            }
        /*Comando rifornimento*/
        } else if(strncmp(comando, "rifornimento", MAX_COMMAND) == 0) {
            useless = scanf("%c", &controllo);
            while(controllo != '\n'){
                useless = scanf("%s ", nome_lot);
                useless = scanf("%d %d", &peso, &scadenza);
                if(scadenza > tempo){      
                    inserisciLotto(facchino, creaLotto(peso, scadenza), nome_lot);
                }
                useless = scanf("%c", &controllo);
            }
            //controlla se ordini in attesa posso andare in ready
            Ricetta * ric;
            Ordine *sentinella = listaAttesa;
            Ordine *prev = NULL;
            while(sentinella != NULL){
                ric = sentinella->ric;
                if(checkMag(ric, sentinella->qnt, tempo)){
                    Ordine *pronto = sentinella;
                    if(prev == NULL){
                        if(sentinella == codaAttesa){
                            codaAttesa = NULL;
                        }
                        listaAttesa = listaAttesa->next;
                        sentinella = listaAttesa;
                    } else {
                        prev->next = pronto->next;
                        sentinella = prev->next;
                    }
                    pronto->next = NULL;
                    
                    insertOrdine(&listaReady, &codaReady, pronto);
                    consumaLotti(facchino, ric, pronto->qnt, tempo);
                } else {
                    prev = sentinella;
                    sentinella = sentinella->next;
                }
            }
            codaAttesa = prev;
            printf("rifornito\n");
        }
        tempo++;
        useless = scanf("%s", comando);
    }
    if(tempo != 0 && periodo != 1 && tempo % periodo == 0){
            if(listaReady == NULL){
                printf("camioncino vuoto\n");
            } else {
/**************************************************************************/
                ord_caricati = 0;
                carico = 0;

                mergeSort(&listaReady);

                //conto quanti ordini entrano nel camioncino
                Ordine *sentinella = listaReady;
                while(sentinella != NULL){
                    if(sentinella != NULL){
                        if(sentinella->peso_tot + carico < capienza){
                        carico += sentinella->peso_tot;
                        ord_caricati++;
                        sentinella = sentinella->next;
                        } else break;                               
                    }
                }
                //funzione per stampare i primi ord_caricati in ordine di peso
                stampaOrdini(listaReady, ord_caricati);   
                for(int j=0; j<ord_caricati; j++){
                    if(listaReady != NULL){
                        listaReady = listaReady->next;
                    }
                }
/***********************************************************************/
            }
            
        }
    (void) useless;

    return 0;
}