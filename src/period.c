#include "matrix.h" // Nécessaire pour t_matrix, mx_copy, mx_mul, mx_free
#include "period.h"
#include <stdlib.h>
#include <stdio.h>

// Définition d'une tolérance pour la comparaison de float
// Ceci est crucial pour comparer des probabilités à zéro en arithmétique flottante
#define EPSILON 1e-6


/**
 * @brief PGCD de deux entiers positifs (algorithme d'Euclide).
 * @param a Premier entier.
 * @param b Deuxième entier.
 * @return int Le Plus Grand Commun Diviseur de a et b.
 */
static int gcd(int a, int b) {
    // L'algorithme d'Euclide est la méthode standard pour trouver le PGCD.
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


/**
 * @brief Calcule la période d'une chaîne de Markov irréductible (une classe SCC).
 *
 * La période d est le PGCD de tous les exposants k pour lesquels une probabilité
 * de retour à l'état de départ est positive (M^k[i][i] > 0).
 *
 * @param[in] MC Matrice de transition (sous-matrice de la classe irréductible)
 *
 * @return Période d(MC) (>= 1). Retourne 0 si la matrice est vide.
 */
int class_period(const t_matrix *MC) {
    if (MC->n <= 0) {
        return 0; // Gère le cas d'une matrice vide
    }

    // Borne supérieure (2*n est un choix pratique et conservateur pour la plupart des cas)
    // Elle limite le nombre d'itérations pour garantir la terminaison rapide.
    const int max_power = 2 * MC->n;
    const int n = MC->n;
    
    // Mk stocke la matrice de transition à la puissance k (commence à M^1)
    t_matrix Mk = (t_matrix){0};
    mx_copy(MC, &Mk); // Mk = M^1
    
    // period_gcd stocke le PGCD mis à jour des exposants k observés
    int period_gcd = 0; 

    // --- Vérification pour k = 1 (La matrice est déjà M^1) ---
    int diag_non_zero = 0;
    for (int i = 0; i < n; i++) {
        // Teste si la probabilité de retour après 1 étape est positive
        if (Mk.a[i][i] > EPSILON) {
            diag_non_zero = 1;
            break;
        }
    }
    if (diag_non_zero) {
        // Si retour possible en 1 étape, la période est au moins 1.
        period_gcd = 1;
    }

    // --- Itération pour k >= 2 (M^2, M^3, ...) ---
    for (int k = 2; k <= max_power; k++) {
        t_matrix Mk_next = (t_matrix){0};
        
        // Calcul de M^k = M^(k-1) * M
        mx_mul(&Mk, MC, &Mk_next);
        
        // Libération de la matrice précédente (M^(k-1))
        mx_free(&Mk);
        Mk = Mk_next; // Mise à jour de Mk pour l'étape k

        diag_non_zero = 0;
        // Vérifie la diagonale de la nouvelle matrice M^k
        for (int i = 0; i < n; i++) {
            // Teste la probabilité de retour après k étapes
            if (Mk.a[i][i] > EPSILON) {
                diag_non_zero = 1;
                break;
            }
        }
        
        if (diag_non_zero) {
            // Un retour est possible après k étapes
            if (period_gcd == 0) {
                // Premier exposant trouvé, initialise le PGCD
                period_gcd = k;
            } else {
                // Met à jour le PGCD avec l'exposant k actuel
                period_gcd = gcd(period_gcd, k);
            }
        }
        
        // Optimisation : si le PGCD atteint 1, la chaîne est apériodique.
        if (period_gcd == 1) {
            mx_free(&Mk);
            return 1;
        }
    }
    
    mx_free(&Mk); // Libère la dernière matrice calculée

    // Cas de la période 1 si aucun retour n'a été trouvé (chaîne apériodique dès le début).
    // Pour une SCC, une période est toujours trouvée, mais cette ligne garantit une valeur >= 1.
    return (period_gcd == 0) ? 1 : period_gcd;
}

/**
 * @brief Indique si la chaîne possède une distribution stationnaire unique.
 *
 * Pour une chaîne irréductible, l'unicité de la distribution stationnaire
 * est garantie si et seulement si elle est apériodique (période = 1).
 *
 * @param[in] MC Matrice de transition (sous-matrice de la classe)
 *
 * @return 1 si la chaîne est apériodique (distribution stationnaire unique), 0 sinon.
 */
int class_has_unique_stationary(const t_matrix *MC) {
    // L'unicité de la distribution stationnaire est équivalente à l'apériodicité.
    return (class_period(MC) == 1);
}
