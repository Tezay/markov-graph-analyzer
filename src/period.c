#include "matrix.h"
#include "period.h"
#include <stdlib.h>
#include <stdio.h>

// Définition d'une tolérance pour la comparaison de float
#define EPSILON 1e-6


/**
 * @brief PGCD de deux entiers positifs (algorithme d'Euclide).
 */
static int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


/**
 * @brief Calcule la période d'une chaîne de Markov irréductible.
 *
 * On élève la matrice de transition M à des puissances successives et on note
 * les exposants k pour lesquels un coefficient diagonal devient non nul.
 * Le PGCD de ces exposants est la période.
 *
 * Différences par rapport à l'exemple de la consigne du projet :
 *  - seuil `EPSILON` pour tester la diagonale au lieu de tester strictement `> 0.0f`;
 *  - borne supérieure choisie à `2 * n` (n = taille) pour attraper les retours
 *    utiles, ce qui reste petit tout en couvrant les cas fréquents;
 *  - PGCD mis à jour à la volée sur les k observés plutôt qu'en stockant
 *    tous les exposants dans un tableau.
 *
 * @param[in] MC  Matrice de transition (supposée fortement connexe)
 *
 * @return  Période d(M) (>= 1). Retourne 0 si la matrice est vide.
 */
int class_period(const t_matrix *MC) {
    if (MC->n <= 0) {
        return 0;
    }

    // Borne supérieure (2*n est un choix pratique)
    const int max_power = 2 * MC->n;
    const int n = MC->n;
    
    t_matrix Mk = (t_matrix){0};
    mx_copy(MC, &Mk); 
    
    int period_gcd = 0; 

    int diag_non_zero = 0;
    for (int i = 0; i < n; i++) {
        if (Mk.a[i][i] > EPSILON) {
            diag_non_zero = 1;
            break;
        }
    }
    if (diag_non_zero) {
        period_gcd = 1;
    }

    for (int k = 2; k <= max_power; k++) {
        t_matrix Mk_next = (t_matrix){0};
        mx_mul(&Mk, MC, &Mk_next);
        
        mx_free(&Mk);
        Mk = Mk_next;

        diag_non_zero = 0;
        for (int i = 0; i < n; i++) {
            if (Mk.a[i][i] > EPSILON) {
                diag_non_zero = 1;
                break;
            }
        }
        
        if (diag_non_zero) {
            if (period_gcd == 0) {
                period_gcd = k;
            } else {
                period_gcd = gcd(period_gcd, k);
            }
        }
        
        if (period_gcd == 1) {
            mx_free(&Mk);
            return 1;
        }
    }
    
    mx_free(&Mk);

    // Si aucune période n'a été trouvée (ce qui est rare pour une SCC), on retourne 1 ou la valeur trouvée
    return (period_gcd == 0) ? 1 : period_gcd;
}

/**
 * @brief Indique si la chaîne possède une distribution stationnaire unique.
 *
 * Théorème classique : pour une chaîne irréductible, l'unicité de la
 * distribution stationnaire est équivalente à l'apériodicité (période = 1).
 *
 * @param[in] MC  Matrice de transition (supposée fortement connexe)
 *
 * @return 1 si la chaîne est apériodique, 0 sinon.
 */
int class_has_unique_stationary(const t_matrix *MC) {
    // Théorème: Une chaîne de Markov irréductible (une SCC) a une unique distribution stationnaire si et seulement si elle est apériodique (période = 1).
    return (class_period(MC) == 1);
}
