#include "clientes.h"

Cliente criarCliente(int id, Filme filmeDesejado, int querPipoca, float paciencia) {
    Cliente cliente = {0};

    cliente.id = id;
    cliente.filmeDesejado = filmeDesejado;
    cliente.querPipoca = querPipoca != 0;
    cliente.paciencia = paciencia > 0.0f ? paciencia : 0.0f;

    return cliente;
}