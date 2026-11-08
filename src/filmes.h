#ifndef FILMES_H
#define FILMES_H

#define MAX_FILMES 8

typedef struct {
    char nome[50];
    int duracao;
    float nota;
    int popularidade;
} Filme;

void carregarFilmes(Filme filmes[], int *quantidade);
void ordenarPorDuracao(Filme filmes[], int quantidade);
void ordenarPorNota(Filme filmes[], int quantidade);
void ordenarPorPopularidade(Filme filmes[], int quantidade);

#endif
