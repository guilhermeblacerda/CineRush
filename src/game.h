#ifndef GAME_H
#define GAME_H

#include "filmes.h"
#include "lista.h"

#define TEMPO_LIMITE 300   /* minutos de tela disponíveis na noite */

// As telas possíveis: o jogo está sempre em exatamente uma delas */
typedef enum {
    TELA_MENU,
    TELA_JOGO,
    TELA_RESULTADO
} Tela;

/* Tudo que o jogo precisa lembrar entre um frame e outro --> 
junta as duas estruturas num lugar só. O cartaz é um vetor porque é fixo. A sessão é uma lista porque cresce e encolhe. */
typedef struct {
    Tela telaAtual;
    Filme cartaz[MAX_FILMES];   // os filmes disponíveis (vetor) */
    int quantidadeCartaz;
    Sessao *sessao;             /* os filmes escolhidos (lista encadeada) */
} Jogo;

void iniciarJogo(void);

#endif