#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scc.h"
#include "hasse.h"
#include "markov_props.h"
#include "mermaid_hasse.h"
#include "utils.h"


/**
 * @brief  Ajoute un lien (from → to) dans un HasseLinkArray avec (re)allocations
 *
 * Alloue le tableau si besoin, double la capacité quand il est plein, puis
 * insère le lien en fin de tableau.
 *
 * @param[in,out]  L    Tableau dynamique de liens (doit être initialisé)
 * @param[in]      from Index de la classe source
 * @param[in]      to   Index de la classe destination
 *
 * @return  void
 *
 * @note    Ne fait rien si `L` est NULL.
 * @see     hasse_init_links(), hasse_free_links()
 */
static void push_link(HasseLinkArray *L, int from, int to) {
    if (!L) return;
    // Allocation initiale si nécessaire
    if (L->capacity == 0) {
        L->capacity = 4;
        L->links = (HasseLink *)malloc(L->capacity * sizeof(HasseLink));
    }
    // Reallocation si plein
    if (L->count >= L->capacity) {
        L->capacity *= 2;
        L->links = (HasseLink *)realloc(L->links, L->capacity * sizeof(HasseLink));
    }
    L->links[L->count].from_class = from;
    L->links[L->count].to_class   = to;
    L->count += 1;
}

/**
 * @brief  Construit une partition de test déterministe
 *
 * Remplit `P` avec 4 classes :
 *  - C0 = {2,4,8,12,24}
 *  - C1 = {3}
 *  - C2 = {7}
 *  - C3 = {35}
 *
 * @param[in,out]  P  Partition à initialiser et remplir
 *
 * @return  void
 *
 * @pre     `P` doit être un pointeur valide.
 * @post    `P` contient 4 classes prêtes à l'emploi.
 * @see     scc_init_partition(), scc_make_empty_class(), scc_add_vertex(), scc_add_class()
 */
static void build_test_partition(Partition *P) {
    scc_init_partition(P);

    // Classe 0
    SccClass c0 = scc_make_empty_class();
    scc_add_vertex(&c0, 2);
    scc_add_vertex(&c0, 4);
    scc_add_vertex(&c0, 8);
    scc_add_vertex(&c0, 12);
    scc_add_vertex(&c0, 24);
    scc_add_class(P, c0);

    // Classe 1
    SccClass c1 = scc_make_empty_class();
    scc_add_vertex(&c1, 3);
    scc_add_class(P, c1);

    // Classe 2
    SccClass c2 = scc_make_empty_class();
    scc_add_vertex(&c2, 7);
    scc_add_class(P, c2);

    // Classe 3
    SccClass c3 = scc_make_empty_class();
    scc_add_vertex(&c3, 35);
    scc_add_class(P, c3);
}

/**
 * @brief  Construit les liens de test entre classes
 *
 * Initialise `L` puis ajoute les arcs :
 *  - C1 → C0
 *  - C2 → C3
 *
 * @param[in,out]  L  Tableau des liens à initialiser et remplir
 *
 * @return  void
 *
 * @see     hasse_init_links(), push_link()
 */
static void build_test_links(HasseLinkArray *L) {
    hasse_init_links(L); // remet count/capacity/ptr à zéro (selon ton implémentation)

    // C1 -> C0
    push_link(L, 1, 0);
    // C2 -> C3
    push_link(L, 2, 3);
}

/**
 * @brief  Affiche joliment un tableau de drapeaux 0/1
 *
 * Formate la sortie comme: `label: [f0, f1, ..., fN-1]`.
 * Si `flags` est NULL, affiche -1 pour chaque case.
 *
 * @param[in]  label  Préfixe affiché avant le tableau
 * @param[in]  flags  Tableau d'entiers 0/1 (ou NULL)
 * @param[in]  n      Taille du tableau
 *
 * @return  void
 */
static void print_flags(const char *label, const int *flags, int n) {
    printf("%s: [", label);
    for (int i = 0; i < n; ++i) {
        printf("%d", flags ? flags[i] : -1);
        if (i + 1 < n) printf(", ");
    }
    printf("]\n");
}

/**
 * @brief  Teste et affiche le caractère absorbant d'une liste de sommets
 *
 * Pour chaque sommet, appelle `markov_is_absorbing_vertex()` et imprime
 * "ABSORBANT (1)" ou "non absorbant (0)".
 *
 * @param[in]  P         Partition en SCC
 * @param[in]  L         Liens du Hasse
 * @param[in]  vertices  Tableau des sommets à tester
 * @param[in]  n         Nombre d’éléments dans `vertices`
 *
 * @return  void
 *
 * @see     markov_is_absorbing_vertex()
 */
static void test_absorbing_vertices(const Partition *P, const HasseLinkArray *L, const int *vertices, int n) {
    for (int i = 0; i < n; ++i) {
        int v = vertices[i];
        int absorb = markov_is_absorbing_vertex(P, L, v);
        printf("  - v=%d : %s\n", v, absorb ? "ABSORBANT (1)" : "non absorbant (0)");
    }
}

/**
 * @brief  Programme de test : analyse de classes et exports
 *
 * Construit une partition et des liens de test, vérifie :
 *  - `markov_class_types`
 *  - `markov_is_irreducible`
 *  - `markov_is_absorbing_vertex` sur quelques sommets
 * puis exporte en Mermaid (.mmd) et texte (.txt), avant nettoyage.
 *
 * @return  Code de retour du processus (0 si succès)
 *
 * @note    Crée le dossier `out` si nécessaire.
 * @see     build_test_partition(), build_test_links(), export_hasse_mermaid(), export_partition_text()
 */
int main(void) {
    printf("=== TEST Partie 2.3 : class_analysis_and_export (types de classes / exports) ===\n");

    // 1) Construire la Partition et les Liens
    Partition P;
    build_test_partition(&P);
    printf("[INFO] Partition construite : %d classes\n", P.count);

    HasseLinkArray L;
    build_test_links(&L);
    printf("[INFO] Liens construits     : %d liens\n", L.count);

    // 2) Tester markov_class_types
    int *is_transient  = (int *)calloc(P.count, sizeof(int));
    int *is_persistent = (int *)calloc(P.count, sizeof(int));
    if (!is_transient || !is_persistent) {
        fprintf(stderr, "[ERR] calloc a échoué pour les indicateurs\n");
        scc_free_partition(&P);
        if (L.links) free(L.links);
        return 1;
    }

    markov_class_types(&L, P.count, is_transient, is_persistent);
    print_flags("is_transient ", is_transient,  P.count);
    print_flags("is_persistent", is_persistent, P.count);

    // 3) Tester markov_is_irreducible
    int irr = markov_is_irreducible(&P);
    printf("markov_is_irreducible(P) = %d (attendu 0)\n", irr);

    // 4) Tester markov_is_absorbing_vertex sur quelques sommets
    //    On sait que 35 (classe 3, taille 1 et pas de sortie) est absorbant.
    //    3 ne l'est pas (sortie vers C0). 24 ne l'est pas (classe multi-éléments).
    const int sample_vertices[] = { 35, 3, 24, 7 };
    printf("Vérification des états absorbants sur {35, 3, 24, 7} :\n");
    test_absorbing_vertices(&P, &L, sample_vertices, (int)(sizeof(sample_vertices)/sizeof(sample_vertices[0])));

    // 5) Tester les exports
    //    Créer le dossier 'out' si besoin (comme dans Partie 1)
    ensure_dir("out");

    const char *file_hasse = "out/hasse_test.mmd";
    const char *file_part  = "out/partition_test.txt";

    if (export_hasse_mermaid(&P, &L, file_hasse) == 0) {
        printf("[OK] Export Hasse Mermaid -> %s\n", file_hasse);
    } else {
        printf("[ERR] Export Hasse Mermaid a échoué\n");
    }

    if (export_partition_text(&P, file_part) == 0) {
        printf("[OK] Export partition (texte) -> %s\n", file_part);
    } else {
        printf("[ERR] Export partition (texte) a échoué\n");
    }

    // 6) Nettoyage
    free(is_transient);
    free(is_persistent);
    scc_free_partition(&P);
    hasse_free_links(&L); // si ton hasse_free_links() fait free(L.links)
    // (sinon : if (L.links) free(L.links);)

    printf("=== FIN TEST class_analysis_and_export ===\n");
    return 0;
}
