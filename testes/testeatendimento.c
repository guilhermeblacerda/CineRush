#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atendimento.h"

int main(void) {
    srand(1);
    Filme filme = {"Filme de teste", 90, 8.0f, 80};
    Sessao *sessao = NULL;
    assert(adicionarFilme(&sessao, filme));

    Atendimento atendimento;
    inicializarAtendimento(&atendimento);
    assert(!iniciarAtendimento(&atendimento, NULL));
    assert(atendimento.encerrado);
    assert(iniciarAtendimento(&atendimento, sessao));
    assert(atendimento.fila.quantidade == 1);
    assert(primeiroCliente(&atendimento.fila)->id == 1);
    assert(strcmp(primeiroCliente(&atendimento.fila)->filmeDesejado.nome,
                  filme.nome) == 0);

    atualizarAtendimento(&atendimento, sessao, -1.0f);
    atualizarAtendimento(&atendimento, sessao, NAN);
    assert(atendimento.tempoRestante == DURACAO_EXPEDIENTE);
    atualizarAtendimento(&atendimento, sessao, 5.0f);
    assert(atendimento.fila.quantidade == 2);
    assert(atenderCliente(&atendimento));
    assert(primeiroCliente(&atendimento.fila)->id == 2);
    assert(atendimento.atendidos == 1);
    assert(atenderCliente(&atendimento));
    assert(!atenderCliente(&atendimento));

    atualizarAtendimento(&atendimento, sessao, 100.0f);
    assert(atendimento.encerrado);
    assert(atendimento.totalCriados == 9);
    assert(atendimento.naoAtendidos == 7);
    assert(filaVazia(&atendimento.fila));
    assert(atendimento.fila.fim == NULL);
    assert(!atenderCliente(&atendimento));

    for (int i = 0; i < 100; i++) {
        assert(iniciarAtendimento(&atendimento, sessao));
        assert(atendimento.atendidos == 0);
        assert(primeiroCliente(&atendimento.fila)->id == 1);
        atualizarAtendimento(&atendimento, sessao, 12.0f);
    }

    liberarAtendimento(&atendimento);
    liberarAtendimento(&atendimento);
    liberarLista(&sessao);
    puts("Todos os testes do atendimento passaram.");
    return 0;
}
