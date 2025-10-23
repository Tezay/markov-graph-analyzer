#include "graph.h"

int main(void) {
    AdjList g;
    graph_init(&g, 4);

    graph_add_edge(&g, 1, 2, 0.5f);
    graph_add_edge(&g, 1, 3, 0.5f);
    graph_add_edge(&g, 2, 4, 1.0f);
    graph_add_edge(&g, 3, 4, 1.0f);

    graph_print(&g);

    graph_free(&g);
    return 0;
}
