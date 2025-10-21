#include <stdio.h> 
#include <stdlib.h>
#include "list.h"

//**
// * @brief Initialise une liste vide
// * 
// * @param l Pointeur vers la liste qui va être créer
// */
void list_inint(List *l) {
    l->head = NULL;
}

//**
// * @brief  Ajoute un nouvel élément au début d'une liste chaînée
// * 
// * @param l      Pointeur vers la liste à modifier
// * @param dest   Sommet de destination à stocker dans la nouvelle cellule
// * @param proba  Probabilité associée à la transition vers `dest`
// */
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

//**
// * @brief  Affiche tous les éléments d'une liste chaînée
// * 
// * @param l  Liste à afficher
// */
void list_print(List l){

    while(l.head != NULL){
        printf("(%d, %f)", l.head->dest, l.head->proba);
        l.head = l.head->next;
    }
}

//**
// * @brief  Libère toute la mémoire allouée pour une liste chaînée
// * 
// * @param l  Pointeur vers la liste à libérer
// */
void list_free(List *l) {
    Cell *cur = l->head;
    Cell *tmp;

    while (cur != NULL) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
}