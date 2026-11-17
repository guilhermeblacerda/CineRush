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
    atendimento->pontos -= atendimento->naoAtendidos * PENALIDADE_CLIENTE;
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
        Cliente *cliente = atendimento->fila.inicio != NULL
                         ? &atendimento->fila.inicio->cliente : NULL;

        if (passo > atendimento->tempoRestante)
            passo = atendimento->tempoRestante;
        if (passo > ateChegada)
            passo = ateChegada;
        if (cliente != NULL && passo > cliente->paciencia)
            passo = cliente->paciencia;

        atendimento->tempoRestante -= passo;
        atendimento->tempoChegada += passo;
        atendimento->tempoAviso -= passo;
        if (cliente != NULL)
            cliente->paciencia -= passo;
        restante -= passo;

        if (atendimento->tempoRestante <= 0.0001f) {
            encerrarAtendimento(atendimento);
            break;
        }

        if (cliente != NULL && cliente->paciencia <= 0.0001f) {
            int id = cliente->id;
            desenfileirar(&atendimento->fila, NULL);
            atendimento->desistentes++;
            atendimento->pontos -= PENALIDADE_CLIENTE;
            snprintf(atendimento->aviso, sizeof(atendimento->aviso),
                     "Cliente %d perdeu a paciencia: -%.0f pontos.",
                     id, PENALIDADE_CLIENTE);
            atendimento->tempoAviso = 2.5f;
        }

        if (atendimento->tempoChegada >= INTERVALO_CHEGADA - 0.0001f) {
            atendimento->tempoChegada = 0.0f;
            gerarCliente(atendimento, sessao);
        }
    }
}

int atenderCliente(Atendimento *atendimento, int oferecerPipoca) {
    if (atendimento == NULL || atendimento->encerrado ||
        (oferecerPipoca != 0 && oferecerPipoca != 1))
        return 0;

    Cliente cliente;

    if (!desenfileirar(&atendimento->fila, &cliente))
        return 0;

    int correto = cliente.querPipoca == oferecerPipoca;

    if (correto) {
        atendimento->atendidos++;
        atendimento->pontos += PONTOS_ACERTO;
        snprintf(atendimento->aviso, sizeof(atendimento->aviso),
                 "Cliente %d atendido corretamente: +%.0f pontos.",
                 cliente.id, PONTOS_ACERTO);
    } else {
        atendimento->errados++;
        atendimento->pontos -= PENALIDADE_CLIENTE;
        snprintf(atendimento->aviso, sizeof(atendimento->aviso),
                 "Pedido do cliente %d estava errado: -%.0f pontos.",
                 cliente.id, PENALIDADE_CLIENTE);
    }

    atendimento->tempoAviso = 2.5f;
    return correto ? 1 : -1;
}

int metaAtendimentoCumprida(const Atendimento *atendimento) {
    return atendimento != NULL && atendimento->encerrado &&
           atendimento->atendidos >= META_ATENDIMENTOS;
}
