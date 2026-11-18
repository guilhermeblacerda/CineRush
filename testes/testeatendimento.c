#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atendimento.h"

static void preencherPedido(Atendimento *a, const Sessao *sessao) {
    const Cliente *cliente = primeiroCliente(&a->fila);
    assert(cliente != NULL);

    int indice = 0;

    while (sessao != NULL &&
           strcmp(sessao->filme.nome, cliente->filmeDesejado.nome) != 0) {
        sessao = sessao->prox;
        indice++;
    }

    assert(sessao != NULL);
    a->filmeSelecionado = indice;
    a->ingressosSelecionados = cliente->quantidadeIngressos;
    a->pipocaSelecionada = cliente->querPipoca;
}

static void conferirPedidoLimpo(const Atendimento *a) {
    assert(a->filmeSelecionado == -1);
    assert(a->ingressosSelecionados == 1);
    assert(a->pipocaSelecionada == 0);
}

static void testarPedidos(const Sessao *sessao) {
    Atendimento a;
    inicializarAtendimento(&a);

    assert(!iniciarAtendimento(&a, NULL, 1));
    assert(iniciarAtendimento(&a, sessao, 1));

    int quantidade = a.fila.quantidade;

    assert(!atenderCliente(&a, sessao));

    preencherPedido(&a, sessao);
    a.filmeSelecionado = 2;
    assert(!atenderCliente(&a, sessao));

    preencherPedido(&a, sessao);
    a.ingressosSelecionados = 0;
    assert(!atenderCliente(&a, sessao));

    preencherPedido(&a, sessao);
    a.pipocaSelecionada = 2;
    assert(!atenderCliente(&a, sessao));

    assert(a.fila.quantidade == quantidade);
    assert(a.pontos == 0.0f);

    preencherPedido(&a, sessao);
    assert(atenderCliente(&a, sessao) == 1);
    assert(a.atendidos == 1 && a.pontos == PONTOS_ACERTO);
    conferirPedidoLimpo(&a);

    for (int campo = 0; campo < 3; campo++) {
        assert(iniciarAtendimento(&a, sessao, 1));
        preencherPedido(&a, sessao);

        if (campo == 0)
            a.filmeSelecionado = 1 - a.filmeSelecionado;
        if (campo == 1)
            a.ingressosSelecionados = 3 - a.ingressosSelecionados;
        if (campo == 2)
            a.pipocaSelecionada = !a.pipocaSelecionada;

        assert(atenderCliente(&a, sessao) == -1);
        assert(a.errados == 1 && a.atendidos == 0);
        assert(a.pontos == -a.penalidadeErro);
        conferirPedidoLimpo(&a);
    }

    liberarAtendimento(&a);
}

static void testarFilaEPaciencia(const Sessao *sessao) {
    Atendimento a;
    inicializarAtendimento(&a);
    assert(iniciarAtendimento(&a, sessao, 1));

    atualizarAtendimento(&a, sessao, 8.0f);

    assert(primeiroCliente(&a.fila)->id == 1);
    assert(fabs(primeiroCliente(&a.fila)->paciencia - 7.0) < 0.01);
    assert(a.fila.inicio->prox != NULL);
    assert(a.fila.inicio->prox->cliente.paciencia == a.pacienciaInicial);

    int proximoId = a.fila.inicio->prox->cliente.id;

    preencherPedido(&a, sessao);
    assert(atenderCliente(&a, sessao) == 1);
    assert(primeiroCliente(&a.fila)->id == proximoId);
    assert(primeiroCliente(&a.fila)->paciencia == a.pacienciaInicial);

    assert(iniciarAtendimento(&a, sessao, 1));
    a.filmeSelecionado = 0;
    atualizarAtendimento(&a, sessao, 15.0f);

    assert(a.desistentes == 1);
    assert(primeiroCliente(&a.fila)->id == 2);
    assert(primeiroCliente(&a.fila)->paciencia == a.pacienciaInicial);
    conferirPedidoLimpo(&a);

    liberarAtendimento(&a);
}

static void testarTempoEReinicio(const Sessao *sessao) {
    Atendimento a;
    inicializarAtendimento(&a);
    assert(iniciarAtendimento(&a, sessao, 1));

    atualizarAtendimento(&a, sessao, -1.0f);
    atualizarAtendimento(&a, sessao, NAN);
    atualizarAtendimento(&a, sessao, INFINITY);

    assert(a.tempoRestante == DURACAO_EXPEDIENTE);

    for (int i = 0; i < 451; i++)
        atualizarAtendimento(&a, sessao, 0.1f);

    assert(a.encerrado && filaVazia(&a.fila));
    assert(a.tempoRestante == 0.0);
    assert(a.totalCriados == a.desistentes + a.naoAtendidos);
    assert(!metaAtendimentoCumprida(&a));

    float pontos = a.pontos;

    atualizarAtendimento(&a, sessao, 100.0f);
    assert(a.pontos == pontos);
    assert(!atenderCliente(&a, sessao));

    assert(iniciarAtendimento(&a, sessao, 1));
    assert(a.pontos == 0.0f && a.atendidos == 0 && a.errados == 0);
    assert(a.desistentes == 0 && a.naoAtendidos == 0 && a.recusados == 0);
    conferirPedidoLimpo(&a);

    while (!a.encerrado) {
        if (!filaVazia(&a.fila)) {
            preencherPedido(&a, sessao);
            assert(atenderCliente(&a, sessao) == 1);
        }

        atualizarAtendimento(&a, sessao, 0.1f);
    }

    assert(a.atendidos == a.totalCriados);
    assert(a.pontos == a.atendidos * PONTOS_ACERTO);
    assert(a.errados == 0 && a.desistentes == 0 && a.naoAtendidos == 0);
    assert(a.recusados == 0 && metaAtendimentoCumprida(&a));

    liberarAtendimento(&a);
    liberarAtendimento(&a);
}


static void testarLimite(const Sessao *sessao) {
    Atendimento a;
    inicializarAtendimento(&a);
    assert(iniciarAtendimento(&a, sessao, 1));

    while (a.fila.quantidade < 3) {
        Cliente cliente = criarCliente(a.proximoId++, sessao->filme, 0, 100.0f);
        assert(enfileirar(&a.fila, cliente));
        a.totalCriados++;
    }

    a.fila.inicio->cliente.paciencia = 100.0;
    a.grupoMinimo = 3;
    a.grupoMaximo = 3;

    atualizarAtendimento(&a, sessao, (float)a.intervaloChegada);

    assert(a.fila.quantidade == CAPACIDADE_FILA);
    assert(a.totalCriados == 4 && a.recusados == 2);
    assert(a.pontos == -2 * a.penalidadeFila);
    assert(primeiroCliente(&a.fila)->id == 1);

    atualizarAtendimento(&a, sessao, (float)a.intervaloChegada);

    assert(a.fila.quantidade == CAPACIDADE_FILA);
    assert(a.totalCriados == 4 && a.recusados == 5);
    assert(a.pontos == -5 * a.penalidadeFila);

    liberarAtendimento(&a);
}


static void testarNiveis(const Sessao *sessao) {
    const float paciencia[] = {15.0f, 12.0f, 10.0f};
    Atendimento a;

    inicializarAtendimento(&a);

    for (int nivel = 1; nivel <= 3; nivel++) {
        assert(iniciarAtendimento(&a, sessao, nivel));
        assert(a.nivel == nivel);
        assert(a.intervaloChegada == 9 - nivel);
        assert(a.pacienciaInicial == paciencia[nivel - 1]);
        assert(a.maxIngressos == nivel + 1);
        assert(a.grupoMinimo == (nivel == 3 ? 2 : 1));
        assert(a.grupoMaximo == (nivel == 1 ? 2 : 3));
        assert(a.penalidadeErro == 5.0f + 5.0f * nivel);
        assert(a.penalidadeFila == 5.0f * nivel);
        assert(a.fila.quantidade >= a.grupoMinimo);
        assert(a.fila.quantidade <= a.grupoMaximo);

        for (const NoCliente *no = a.fila.inicio;
             no != NULL; no = no->prox) {
            assert(no->cliente.quantidadeIngressos >= 1);
            assert(no->cliente.quantidadeIngressos <= a.maxIngressos);
        }
    }

    liberarAtendimento(&a);
}

int main(void) {
    srand(1);

    Filme filmes[] = {
        {"Filme A", 80, 8.0f, 80},
        {"Filme B", 90, 9.0f, 90}
    };

    Sessao *sessao = NULL;

    assert(adicionarFilme(&sessao, filmes[0]));
    assert(adicionarFilme(&sessao, filmes[1]));

    Cliente cliente = criarCliente(1, filmes[0], 1, 10.0f);
    assert(cliente.quantidadeIngressos == 1);

    assert(filmeDaProgramacao(sessao, 0) != NULL);
    assert(filmeDaProgramacao(sessao, -1) == NULL);
    assert(filmeDaProgramacao(sessao, 2) == NULL);

    testarPedidos(sessao);
    testarFilaEPaciencia(sessao);
    testarTempoEReinicio(sessao);
    testarLimite(sessao);
    testarNiveis(sessao);

    liberarLista(&sessao);
    puts("Todos os testes do atendimento passaram.");
    return 0;
}
