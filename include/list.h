#ifndef LIST_H
#define LIST_H

typedef struct Cell {
    int   dest;          // sommet d’arrivée
    float proba;         // probabilité de transition
    struct Cell *next;   // cellule suivante
} Cell;

typedef struct {
    Cell *head;          // tête de liste
} List;

// API liste
void  list_init(List *l);
void  list_push_front(List *l, int dest, float proba);
void  list_print(const List *l);
void  list_free(List *l);

#endif