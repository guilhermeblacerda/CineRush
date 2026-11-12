#ifndef CLIENTES_H
#define CLIENTES_H

#include "filmes.h"

typedef struct {
    int id;
    Filme filmeDesejado;
    int querPipoca;
    float paciencia;
} Cliente;

Cliente criarCliente(int id, Filme filmeDesejado, int querPipoca, float paciencia);

#endif