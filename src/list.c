#include <stdio.h>
#include "list.h"

void list_inint(List *l) {
    l->head = NULL;
}

void  list_push_front(List *l, int dest, float proba){
    Cell *new = malloc(sizeof(Cell));
    if (!new) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new->dest = dest;
    new->proba = proba;
    new->next = l->head;
    l->head = new;
}

void list_print(List l){
    while(l != NULL){
        printf("(%d, %f)", l.head->dest, l.head->proba);
        l.head = l.head->next;
    }
}