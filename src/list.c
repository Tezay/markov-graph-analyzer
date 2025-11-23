#include <stdio.h> 
#include <stdlib.h>
#include "list.h"

/**
 * @brief Initialise une liste vide
 * @param l Pointeur vers la liste qui va être créer
 */
void list_init(List *l) {
    // Initialisation : La tête de la liste pointe vers NULL, indiquant qu'elle est vide.
    l->head = NULL;
}

/**
 * @brief  Ajoute un nouvel élément au début d'une liste chaînée (Push Front)
 * @param l      Pointeur vers la liste à modifier
 * @param dest   Sommet de destination à stocker dans la nouvelle cellule
 * @param proba  Probabilité associée à la transition vers `dest`
 */
void  list_push_front(List *l, int dest, float proba){
    // 1. Allocation mémoire pour la nouvelle cellule (Cellule = Transition)
    Cell *new = malloc(sizeof(Cell));
    
    // Vérification de l'allocation
    if (!new) {
        perror("malloc"); // Affiche le message d'erreur système
        exit(EXIT_FAILURE); // Arrête le programme en cas d'échec critique
    }

    // 2. Initialisation des données de la nouvelle cellule
    new->dest = dest;
    new->proba = proba;
    
    // 3. Liaison : Le 'next' de la nouvelle cellule pointe vers l'ancienne tête (l->head)
    new->next = l->head;
    
    // 4. Mise à jour : La nouvelle cellule devient la nouvelle tête de la liste
    l->head = new;
}

/**
 * @brief  Affiche tous les éléments d'une liste chaînée
 * @param l  Liste à afficher (Passage par valeur, la liste originale n'est pas modifiée)
 */
void list_print(List l){
    // Parcourt la liste jusqu'à la fin
    while(l.head != NULL){
        // Affiche la transition (Destination, Probabilité)
        printf("(%d, %f)", l.head->dest, l.head->proba);
        // Passe à l'élément suivant
        l.head = l.head->next;
    }
}

/**
 * @brief  Libère toute la mémoire allouée pour une liste chaînée
 * @param l  Pointeur vers la liste à libérer (Passage par référence)
 */
void list_free(List *l) {
    Cell *cur = l->head; // Pointeur courant
    Cell *tmp; // Pointeur temporaire pour stocker le suivant avant de libérer

    while (cur != NULL) {
        tmp = cur->next; // Stocke la cellule suivante AVANT de libérer 'cur'
        free(cur);       // Libère la mémoire de la cellule actuelle
        cur = tmp;       // Avance au pointeur suivant stocké
    }
}