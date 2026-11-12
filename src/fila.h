#ifndef FILA_H
#define FILA_H

#include "clientes.h"

typedef struct NoCliente {
    Cliente cliente;
    struct NoCliente *prox;
} NoCliente;

typedef struct {
    NoCliente *inicio;
    NoCliente *fim;
    int quantidade;
} FilaClientes;

void inicializarFila(FilaClientes *fila);
int filaVazia(const FilaClientes *fila);
int enfileirar(FilaClientes *fila, Cliente cliente);
int desenfileirar(FilaClientes *fila, Cliente *cliente);
const Cliente *primeiroCliente(const FilaClientes *fila);
void liberarFila(FilaClientes *fila);

#endif