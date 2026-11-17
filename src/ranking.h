
#ifndef RANKING_H
#define RANKING_H

#define MAX_RANKING 10
#define TAM_NOME 30

typedef struct {
    char nome[TAM_NOME];
    float pontuacao;
} Jogador;

void carregarRanking(Jogador ranking[], int *quantidade);

void salvarRanking(const Jogador ranking[], int quantidade);

void ordenarRanking(Jogador ranking[], int quantidade);

void adicionarRanking(
    Jogador ranking[],
    int *quantidade,
    const char *nome,
    float pontuacao
);

#endif
