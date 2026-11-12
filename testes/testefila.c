
#include <stdio.h>
#include "clientes.h"
#include "fila.h"
#include "filmes.h"

int main(void) {
    Filme filmes[MAX_FILMES];
    int quantidade;

    carregarFilmes(filmes, &quantidade);

    FilaClientes fila;
    inicializarFila(&fila);

    Cliente c1 = criarCliente(1, filmes[0], 1, 30.0f);
    Cliente c2 = criarCliente(2, filmes[1], 0, 20.0f);
    Cliente c3 = criarCliente(3, filmes[2], 1, 15.0f);

    enfileirar(&fila, c1);
    enfileirar(&fila, c2);
    enfileirar(&fila, c3);

    printf("Clientes na fila: %d\n", fila.quantidade);

    Cliente atendido;

    while (desenfileirar(&fila, &atendido)) {
        printf("Cliente %d atendido | Filme: %s | Pipoca: %s\n",
               atendido.id,
               atendido.filmeDesejado.nome,
               atendido.querPipoca ? "Sim" : "Nao");
    }

    printf("Clientes restantes: %d\n", fila.quantidade);

    liberarFila(&fila);

    return 0;
}
