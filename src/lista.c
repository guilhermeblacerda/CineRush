#include <stdlib.h>
#include "lista.h"

void inicializarLista(Sessao **inicio) {
    *inicio = NULL;
}

int adicionarFilme(Sessao **inicio, Filme filme) {
    Sessao *novo = malloc(sizeof(Sessao));
    if (novo == NULL) return 0;

    novo->filme = filme;
    novo->prox = NULL;

    if (*inicio == NULL) {
        *inicio = novo;
    } else {
        Sessao *atual = *inicio;
        while (atual->prox != NULL) atual = atual->prox;
        atual->prox = novo;
    }

    return 1;
}

int removerFilme(Sessao **inicio, int indice) {
    if (*inicio == NULL || indice < 0) return 0;

    Sessao *atual = *inicio;

    if (indice == 0) {
        *inicio = atual->prox;
        free(atual);
        return 1;
    }

    for (int i = 0; atual != NULL && i < indice - 1; i++) {
        atual = atual->prox;
    }

    if (atual == NULL || atual->prox == NULL) return 0;

    Sessao *removido = atual->prox;
    atual->prox = removido->prox;
    free(removido);
    return 1;
}

int calcularDuracaoTotal(Sessao *inicio) {
    int total = 0;
    while (inicio != NULL) {
        total += inicio->filme.duracao;
        inicio = inicio->prox;
    }
    return total;
}

float calcularPontuacao(Sessao *inicio) {
    float total = 0.0f;
    while (inicio != NULL) {
        total += inicio->filme.nota * 10.0f
               + inicio->filme.popularidade * 0.2f;
        inicio = inicio->prox;
    }
    return total;
}

int quantidadeSelecionada(Sessao *inicio) {
    int total = 0;
    while (inicio != NULL) {
        total++;
        inicio = inicio->prox;
    }
    return total;
}

void liberarLista(Sessao **inicio) {
    while (*inicio != NULL) {
        Sessao *removido = *inicio;
        *inicio = (*inicio)->prox;
        free(removido);
    }
}
