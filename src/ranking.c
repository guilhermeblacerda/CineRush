
#include <stdio.h>
#include <string.h>
#include "ranking.h"

void ordenarRanking(Jogador ranking[], int quantidade) {
    for (int i = 1; i < quantidade; i++) {
        Jogador chave = ranking[i];
        int j = i - 1;

        while (j >= 0 && ranking[j].pontuacao < chave.pontuacao) {
            ranking[j + 1] = ranking[j];
            j--;
        }

        ranking[j + 1] = chave;
    }
}

void carregarRanking(Jogador ranking[], int *quantidade) {
    FILE *arquivo = fopen("ranking.txt", "r");

    *quantidade = 0;

    if (arquivo == NULL) {
        return;
    }

    char linha[100];

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Jogador jogador;

        if (sscanf(linha, "%29[^;];%f",
                   jogador.nome,
                   &jogador.pontuacao) == 2) {

            if (*quantidade < MAX_RANKING) {
                ranking[*quantidade] = jogador;
                (*quantidade)++;
            }
        }
    }

    fclose(arquivo);

    ordenarRanking(ranking, *quantidade);
}

void salvarRanking(const Jogador ranking[], int quantidade) {
    FILE *arquivo = fopen("ranking.txt", "w");

    if (arquivo == NULL) {
        return;
    }

    for (int i = 0; i < quantidade; i++) {
        fprintf(arquivo, "%s;%.1f\n",
                ranking[i].nome,
                ranking[i].pontuacao);
    }

    fclose(arquivo);
}

void adicionarRanking(
    Jogador ranking[],
    int *quantidade,
    const char *nome,
    float pontuacao
) {
    if (*quantidade == MAX_RANKING &&
        pontuacao <= ranking[MAX_RANKING - 1].pontuacao) {
        return;
    }

    if (*quantidade < MAX_RANKING) {
        (*quantidade)++;
    }

    int posicao = *quantidade - 1;

    while (posicao > 0 &&
           ranking[posicao - 1].pontuacao < pontuacao) {
        ranking[posicao] = ranking[posicao - 1];
        posicao--;
    }

    strncpy(ranking[posicao].nome, nome, TAM_NOME - 1);
    ranking[posicao].nome[TAM_NOME - 1] = '\0';
    ranking[posicao].pontuacao = pontuacao;
}
