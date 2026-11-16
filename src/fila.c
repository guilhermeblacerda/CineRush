#include <stdlib.h>
#include "fila.h"

void inicializarFila(FilaClientes *fila) {
    if (fila == NULL) return;

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

int filaVazia(const FilaClientes *fila) {
    return fila == NULL || fila->inicio == NULL;
}

int enfileirar(FilaClientes *fila, Cliente cliente) {
    if (fila == NULL) return 0;

    NoCliente *novo = malloc(sizeof(NoCliente));
    if (novo == NULL) return 0;

    novo->cliente = cliente;
    novo->prox = NULL;

    if (fila->fim == NULL) {
        fila->inicio = novo;
    } else {
        fila->fim->prox = novo;
    }

    fila->fim = novo;
    fila->quantidade++;

    return 1;
}

int desenfileirar(FilaClientes *fila, Cliente *cliente) {
    if (filaVazia(fila)) return 0;

    NoCliente *removido = fila->inicio;

    if (cliente != NULL) {
        *cliente = removido->cliente;
    }

    fila->inicio = removido->prox;

    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    fila->quantidade--;
    free(removido);

    return 1;
}

const Cliente *primeiroCliente(const FilaClientes *fila) {
    if (filaVazia(fila)) return NULL;

    return &fila->inicio->cliente;
}

void liberarFila(FilaClientes *fila) {
    while (!filaVazia(fila)) {
        desenfileirar(fila, NULL);
    }
}