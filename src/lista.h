#ifndef LISTA_H
#define LISTA_H

#include "filmes.h"

typedef struct Sessao {
    Filme filme;
    struct Sessao *prox;
} Sessao;

void inicializarLista(Sessao **inicio);
int adicionarFilme(Sessao **inicio, Filme filme);
int removerFilme(Sessao **inicio, int indice);
int calcularDuracaoTotal(Sessao *inicio);
float calcularPontuacao(Sessao *inicio);
int quantidadeSelecionada(Sessao *inicio);
void liberarLista(Sessao **inicio);

#endif
