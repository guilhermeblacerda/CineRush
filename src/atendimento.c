#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atendimento.h"

static void limparPedido(Atendimento *a) {
    a->filmeSelecionado = -1;
    a->ingressosSelecionados = 1;
    a->pipocaSelecionada = 0;
}

static void configurarAtendimento(Atendimento *a, int nivel) {
    a->nivel = nivel < 1 ? 1 : (nivel > 3 ? 3 : nivel);

    const float paciencia[] = {15.0f, 12.0f, 10.0f};

    a->intervaloChegada = 9.0 - a->nivel;
    a->pacienciaInicial = paciencia[a->nivel - 1];
    a->maxIngressos = a->nivel + 1;
    a->grupoMinimo = a->nivel == 3 ? 2 : 1;
    a->grupoMaximo = a->nivel == 1 ? 2 : 3;
    a->penalidadeErro = 5.0f + 5.0f * a->nivel;
    a->penalidadeFila = 5.0f * a->nivel;
}

const Filme *filmeDaProgramacao(const Sessao *sessao, int indice) {
    if (indice < 0)
        return NULL;

    while (sessao != NULL && indice > 0) {
        sessao = sessao->prox;
        indice--;
    }

    return sessao != NULL ? &sessao->filme : NULL;
}

static int gerarCliente(Atendimento *a, const Sessao *sessao) {
    int quantidade = 0;

    for (const Sessao *no = sessao; no != NULL; no = no->prox)
        quantidade++;

    if (quantidade == 0)
        return 0;

    const Filme *filme = filmeDaProgramacao(sessao, rand() % quantidade);
    Cliente cliente = criarCliente(a->proximoId, *filme,
                                   rand() % 2, a->pacienciaInicial);

    cliente.quantidadeIngressos = 1 + rand() % a->maxIngressos;

    if (!enfileirar(&a->fila, cliente)) {
        snprintf(a->aviso, sizeof(a->aviso), "Falha ao criar cliente.");
        a->tempoAviso = 2.5;
        return 0;
    }

    a->proximoId++;
    a->totalCriados++;
    return 1;
}

static int gerarChegada(Atendimento *a, const Sessao *sessao) {
    int quantidade = a->grupoMinimo +
                     rand() % (a->grupoMaximo - a->grupoMinimo + 1);
    int entraram = 0;
    int recusados = 0;

    for (int i = 0; i < quantidade; i++) {
        if (a->fila.quantidade >= CAPACIDADE_FILA) {
            a->recusados++;
            recusados++;
            a->pontos -= a->penalidadeFila;
        } else if (gerarCliente(a, sessao)) {
            entraram++;
        } else {
            break;
        }
    }

    if (recusados > 0) {
        snprintf(a->aviso, sizeof(a->aviso),
                 "Fila cheia: %d entrou/entraram, %d recusado(s). -%.0f pontos.",
                 entraram, recusados, recusados * a->penalidadeFila);
        a->tempoAviso = 2.5;
    } else if (entraram > 0 && a->tempoAviso <= 0.0) {
        snprintf(a->aviso, sizeof(a->aviso),
                 "Chegaram %d cliente(s).", entraram);
        a->tempoAviso = 2.5;
    }

    return entraram;
}

static void encerrarAtendimento(Atendimento *a) {
    if (a->encerrado)
        return;

    a->naoAtendidos = a->fila.quantidade;
    a->pontos -= a->naoAtendidos * a->penalidadeErro;
    liberarFila(&a->fila);
    limparPedido(a);
    a->tempoRestante = 0.0;
    a->encerrado = 1;
}

void inicializarAtendimento(Atendimento *a) {
    if (a == NULL)
        return;

    *a = (Atendimento){0};
    inicializarFila(&a->fila);
    configurarAtendimento(a, 1);
    limparPedido(a);
    a->proximoId = 1;
    a->encerrado = 1;
}

void liberarAtendimento(Atendimento *a) {
    if (a == NULL)
        return;

    liberarFila(&a->fila);
    a->encerrado = 1;
}

int iniciarAtendimento(Atendimento *a, const Sessao *sessao, int nivel) {
    if (a == NULL || sessao == NULL)
        return 0;

    liberarAtendimento(a);
    inicializarAtendimento(a);
    configurarAtendimento(a, nivel);
    a->tempoRestante = DURACAO_EXPEDIENTE;
    a->encerrado = 0;

    if (!gerarChegada(a, sessao)) {
        liberarAtendimento(a);
        return 0;
    }

    return 1;
}

void atualizarAtendimento(Atendimento *a, const Sessao *sessao, float dt) {
    if (a == NULL || a->encerrado || !isfinite(dt) || dt <= 0.0f)
        return;

    double restante = dt;
    const double epsilon = 0.00001;

    while (restante > epsilon && !a->encerrado) {
        double passo = restante;
        double ateChegada = a->intervaloChegada - a->tempoChegada;
        int recebeClientes = a->tempoRestante - ateChegada >
                             MARGEM_FECHAMENTO + epsilon;
        Cliente *cliente = a->fila.inicio != NULL
                         ? &a->fila.inicio->cliente : NULL;

        if (passo > a->tempoRestante)
            passo = a->tempoRestante;
        if (recebeClientes && passo > ateChegada)
            passo = ateChegada;
        if (cliente != NULL && passo > cliente->paciencia)
            passo = cliente->paciencia;

        a->tempoRestante -= passo;
        a->tempoChegada += passo;
        a->tempoAviso -= passo;

        if (cliente != NULL)
            cliente->paciencia -= passo;

        restante -= passo;

        if (a->tempoRestante <= epsilon) {
            encerrarAtendimento(a);
            break;
        }

        if (cliente != NULL && cliente->paciencia <= epsilon) {
            int id = cliente->id;
            desenfileirar(&a->fila, NULL);
            a->desistentes++;
            a->pontos -= a->penalidadeErro;
            limparPedido(a);

            snprintf(a->aviso, sizeof(a->aviso),
                     "Cliente %d desistiu: -%.0f pontos.",
                     id, a->penalidadeErro);
            a->tempoAviso = 2.5;
        }

        if (recebeClientes &&
            a->tempoChegada >= a->intervaloChegada - epsilon) {
            a->tempoChegada = 0.0;
            gerarChegada(a, sessao);
        }
    }
}

int atenderCliente(Atendimento *a, const Sessao *sessao) {
    if (a == NULL || a->encerrado || filaVazia(&a->fila))
        return 0;

    const Filme *filme = filmeDaProgramacao(sessao, a->filmeSelecionado);

    if (filme == NULL || a->ingressosSelecionados < 1 ||
        a->ingressosSelecionados > a->maxIngressos ||
        (a->pipocaSelecionada != 0 && a->pipocaSelecionada != 1)) {
        snprintf(a->aviso, sizeof(a->aviso),
                 "Escolha um filme e uma quantidade valida antes de entregar.");
        a->tempoAviso = 2.5;
        return 0;
    }

    Cliente cliente;

    if (!desenfileirar(&a->fila, &cliente))
        return 0;

    int correto = strcmp(cliente.filmeDesejado.nome, filme->nome) == 0 &&
                  cliente.quantidadeIngressos == a->ingressosSelecionados &&
                  cliente.querPipoca == a->pipocaSelecionada;

    if (correto) {
        a->atendidos++;
        a->pontos += PONTOS_ACERTO;
    } else {
        a->errados++;
        a->pontos -= a->penalidadeErro;
    }

    snprintf(a->aviso, sizeof(a->aviso),
             "Cliente %d: pedido %s (%+.0f pontos).", cliente.id,
             correto ? "correto" : "errado",
             correto ? PONTOS_ACERTO : -a->penalidadeErro);

    a->tempoAviso = 2.5;
    limparPedido(a);
    return correto ? 1 : -1;
}

int metaAtendimentoCumprida(const Atendimento *a) {
    return a != NULL && a->encerrado &&
           a->atendidos >= META_ATENDIMENTOS;
}
