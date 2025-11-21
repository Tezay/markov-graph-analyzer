#include "matrix.h"
#include "period.h"
#include <stdlib.h>
#include <stdio.h>

// Définition d'une tolérance pour la comparaison de flottants
#define EPSILON 1e-6


static int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}


int class_period(const t_matrix *MC) {
    if (MC->n <= 0) {
        return 0;
    }

    // Borne supérieure (2*n est un choix pratique)
    const int max_power = 2 * MC->n;
    const int n = MC->n;
    
    t_matrix Mk;
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
        t_matrix Mk_next;
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

int class_has_unique_stationary(const t_matrix *MC) {
    // Théorème: Une chaîne de Markov irréductible (une SCC) a une unique distribution stationnaire si et seulement si elle est apériodique (période = 1).
    return (class_period(MC) == 1);
}