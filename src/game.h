
#ifndef GAME_H
#define GAME_H

#include "filmes.h"
#include "lista.h"

#define TEMPO_LIMITE 300

typedef enum {
    TELA_MENU,
    TELA_DESAFIOS,
    TELA_JOGO,
    TELA_RESULTADO
} Tela;

typedef enum {
    NOITE_DOS_CRITICOS,
    SESSAO_DE_SUCESSO,
    TEMPO_E_OURO,
    FESTIVAL_DIVERSIFICADO,
    MARATONA_CINEMATOGRAFICA,
    OBRAS_PRIMAS,
    TOTAL_DESAFIOS
} Desafio;


typedef struct {
    Tela telaAtual;
    Filme cartaz[MAX_FILMES];
    int quantidadeCartaz;
    Sessao *sessao;

    Desafio desafioAtual;
    int desafioAnterior;
} Jogo;

void iniciarJogo(void);

#endif
