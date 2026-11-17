#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "atendimento.h"

static int gerarCliente(Atendimento *atendimento, const Sessao *sessao) {
    int quantidade = 0;

    for (const Sessao *no = sessao; no != NULL; no = no->prox)
        quantidade++;

    if (quantidade == 0)
        return 0;

    int indice = rand() % quantidade;
    const Sessao *escolhida = sessao;

    while (indice-- > 0)
        escolhida = escolhida->prox;

    Cliente cliente = criarCliente(atendimento->proximoId,
                                   escolhida->filme,
                                   rand() % 2,
                                   PACIENCIA_CLIENTE);

    if (!enfileirar(&atendimento->fila, cliente)) {
        snprintf(atendimento->aviso, sizeof(atendimento->aviso),
                 "Nao foi possivel adicionar outro cliente.");
        atendimento->tempoAviso = 2.5f;
        return 0;
    }

    atendimento->proximoId++;
    atendimento->totalCriados++;

    if (atendimento->tempoAviso <= 0.0f) {
        snprintf(atendimento->aviso, sizeof(atendimento->aviso),
                 "Cliente %d entrou na fila.", cliente.id);
        atendimento->tempoAviso = 2.5f;
    }

    return 1;
}

static void encerrarAtendimento(Atendimento *atendimento) {
    if (atendimento->encerrado)
        return;

    atendimento->naoAtendidos = atendimento->fila.quantidade;
    liberarFila(&atendimento->fila);
    atendimento->tempoRestante = 0.0f;
    atendimento->encerrado = 1;
}

void inicializarAtendimento(Atendimento *atendimento) {
    if (atendimento == NULL)
        return;

    *atendimento = (Atendimento){0};
    inicializarFila(&atendimento->fila);
    atendimento->proximoId = 1;
    atendimento->encerrado = 1;
}

void liberarAtendimento(Atendimento *atendimento) {
    if (atendimento == NULL)
        return;

    liberarFila(&atendimento->fila);
    atendimento->encerrado = 1;
}

int iniciarAtendimento(Atendimento *atendimento, const Sessao *sessao) {
    if (atendimento == NULL || sessao == NULL)
        return 0;

    liberarAtendimento(atendimento);
    inicializarAtendimento(atendimento);
    atendimento->tempoRestante = DURACAO_EXPEDIENTE;
    atendimento->encerrado = 0;

    if (!gerarCliente(atendimento, sessao)) {
        liberarAtendimento(atendimento);
        return 0;
    }

    return 1;
}

void atualizarAtendimento(Atendimento *atendimento, const Sessao *sessao, float dt) {
    if (atendimento == NULL || atendimento->encerrado ||
        !isfinite(dt) || dt <= 0.0f)
        return;

    double restante = dt;
    while (restante > 0.0 && !atendimento->encerrado) {
        double passo = restante;
        double ateChegada = INTERVALO_CHEGADA - atendimento->tempoChegada;

        if (passo > atendimento->tempoRestante)
            passo = atendimento->tempoRestante;
        if (passo > ateChegada)
            passo = ateChegada;

        atendimento->tempoRestante -= passo;
        atendimento->tempoChegada += passo;
        atendimento->tempoAviso -= passo;
        restante -= passo;

        if (atendimento->tempoRestante <= 0.0001f) {
            encerrarAtendimento(atendimento);
            break;
        }

        if (atendimento->tempoChegada >= INTERVALO_CHEGADA - 0.0001f) {
            atendimento->tempoChegada = 0.0f;
            gerarCliente(atendimento, sessao);
        }
    }
}

int atenderCliente(Atendimento *atendimento) {
    if (atendimento == NULL || atendimento->encerrado)
        return 0;

    Cliente cliente;

    if (!desenfileirar(&atendimento->fila, &cliente))
        return 0;

    atendimento->atendidos++;
    snprintf(atendimento->aviso, sizeof(atendimento->aviso),
             "Cliente %d atendido. O proximo da fila foi chamado.", cliente.id);
    atendimento->tempoAviso = 2.5f;
    return 1;
}
