#include "filmes.h"

void carregarFilmes(Filme filmes[], int *quantidade) {
    Filme dados[] = {
        {"Aventura Espacial", 120, 9.0f, 90},
        {"O Misterio", 90, 8.5f, 75},
        {"Comedia Total", 100, 7.0f, 85},
        {"Ultima Viagem", 80, 9.5f, 95},
        {"Noite Sombria", 110, 8.0f, 70},
        {"O Grande Golpe", 95, 8.7f, 88},
        {"Mundo Perdido", 130, 8.2f, 82},
        {"Um Dia de Sol", 75, 7.8f, 80}
    };

    *quantidade = sizeof(dados) / sizeof(dados[0]);

    for (int i = 0; i < *quantidade; i++) {
        filmes[i] = dados[i];
    }
}

void ordenarPorDuracao(Filme filmes[], int quantidade) {
    for (int i = 1; i < quantidade; i++) {
        Filme atual = filmes[i];
        int j = i - 1;

        while (j >= 0 && filmes[j].duracao > atual.duracao) {
            filmes[j + 1] = filmes[j];
            j--;
        }

        filmes[j + 1] = atual;
    }
}

void ordenarPorNota(Filme filmes[], int quantidade) {
    for (int i = 1; i < quantidade; i++) {
        Filme atual = filmes[i];
        int j = i - 1;

        while (j >= 0 && filmes[j].nota < atual.nota) {
            filmes[j + 1] = filmes[j];
            j--;
        }

        filmes[j + 1] = atual;
    }
}

void ordenarPorPopularidade(Filme filmes[], int quantidade) {
    for (int i = 1; i < quantidade; i++) {
        Filme atual = filmes[i];
        int j = i - 1;

        while (j >= 0 && filmes[j].popularidade < atual.popularidade) {
            filmes[j + 1] = filmes[j];
            j--;
        }

        filmes[j + 1] = atual;
    }
}
