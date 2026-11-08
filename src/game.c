/*
 * game.c - Loop principal do CineRush.
 *
 * O jogo se apresenta como uma MAQUINA DE ESTADOS: existe um unico loop rodando 60 vezes
 * por segundo, e a variavel jogo.telaAtual --> no .h diz qual tela esta ativa.
 * 
 * 
 * A cada frame fazemos duas coisas:
 *   1. ATUALIZAR: ler o teclado e mudar os dados do jogo;
 *   2. DESENHAR: mostrar na janela o estado atual.
 *
 *
 * Obs.: a fonte padrao da raylib nao tem acentos, por isso os textos
 * desenhados na tela estao sem acento.
 */

#include <string.h>
#include "raylib.h"
#include "game.h"

/* ============================ AUXILIARES ============================ */

/* Deixa o jogo pronto para uma partida nova: cartaz cheio e sessao vazia. */
static void prepararPartida(Jogo *jogo) {
    carregarFilmes(jogo->cartaz, &jogo->quantidadeCartaz);
    liberarLista(&jogo->sessao);      /* apaga a sessao da partida anterior */
    inicializarLista(&jogo->sessao);
}


//-----------------------------------%----------------------------------------

/* Retorna 1 se o filme (comparado pelo nome) ja esta na sessao. */
static int filmeJaNaSessao(Sessao *inicio, const char *nome) {
    while (inicio != NULL) {
        if (strcmp(inicio->filme.nome, nome) == 0) return 1;
        inicio = inicio->prox;
    }
    return 0;
}

/* ============================ ATUALIZAR ============================ */

/* MENU: ENTER comeca uma partida nova. */
static void atualizarMenu(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        prepararPartida(jogo);
        jogo->telaAtual = TELA_JOGO;
    }
}

//------------------------------%---------------------------------------------

/* PARTIDA: escolher, remover e ordenar filmes; ENTER encerra a noite. */
static void atualizarPartida(Jogo *jogo) {
    /* Teclas 1 a 8: tenta colocar o filme daquela posicao na sessao */
    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            Filme escolhido = jogo->cartaz[i];
            int tempoUsado = calcularDuracaoTotal(jogo->sessao);
            int cabeNoTempo = (tempoUsado + escolhido.duracao) <= TEMPO_LIMITE;

            if (cabeNoTempo && !filmeJaNaSessao(jogo->sessao, escolhido.nome)) {
                adicionarFilme(&jogo->sessao, escolhido);
            }
        }
    }

    /* BACKSPACE: tira o ultimo filme da sessao (lista vazia: nao faz nada) */
    if (IsKeyPressed(KEY_BACKSPACE)) {
        int ultimo = quantidadeSelecionada(jogo->sessao) - 1;
        removerFilme(&jogo->sessao, ultimo);
    }

    /* D, N, P: reordenam o cartaz para ajudar o jogador a decidir */
    if (IsKeyPressed(KEY_D)) ordenarPorDuracao(jogo->cartaz, jogo->quantidadeCartaz);
    if (IsKeyPressed(KEY_N)) ordenarPorNota(jogo->cartaz, jogo->quantidadeCartaz);
    if (IsKeyPressed(KEY_P)) ordenarPorPopularidade(jogo->cartaz, jogo->quantidadeCartaz);

    /* ENTER: fecha a programacao da noite e mostra o resultado */
    if (IsKeyPressed(KEY_ENTER)) {
        jogo->telaAtual = TELA_RESULTADO;
    }
}

/* RESULTADO: ENTER volta ao menu. */
static void atualizarResultado(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        jogo->telaAtual = TELA_MENU;
    }
}

/* ============================= DESENHAR ============================= */

static void desenharMenu(void) {
    DrawText("CINERUSH", 380, 200, 80, MAROON);
    DrawText("Monte a sessao da noite em ate 300 minutos", 300, 320, 24, DARKGRAY);
    DrawText("e faca a maior pontuacao possivel.", 360, 355, 24, DARKGRAY);
    DrawText("ENTER para comecar   |   ESC para sair", 330, 470, 22, GRAY);
}

//-----------------------------%---------------------------------------------

static void desenharPartida(const Jogo *jogo) {
    /* Coluna da esquerda: o cartaz (vetor) */
    DrawText("CARTAZ", 40, 30, 30, MAROON);
    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        Filme f = jogo->cartaz[i];
        DrawText(TextFormat("%d) %-20s %3d min  nota %.1f  pop %d",
                            i + 1, f.nome, f.duracao, f.nota, f.popularidade),
                 40, 80 + i * 40, 20, DARKGRAY);
    }

    /* Coluna da direita: a sessao montada (lista encadeada) */
    DrawText("SUA SESSAO", 640, 30, 30, DARKBLUE);
    int linha = 0;
    for (Sessao *no = jogo->sessao; no != NULL; no = no->prox) {
        DrawText(TextFormat("- %s (%d min)", no->filme.nome, no->filme.duracao),
                 640, 80 + linha * 40, 20, DARKGRAY);
        linha++;
    }

    /* Rodape: placar e controles */
    int tempoUsado = calcularDuracaoTotal(jogo->sessao);
    DrawText(TextFormat("Tempo: %d / %d min", tempoUsado, TEMPO_LIMITE), 40, 560, 24, BLACK);
    DrawText(TextFormat("Pontos: %.1f", calcularPontuacao(jogo->sessao)), 640, 560, 24, BLACK);
    DrawText("1-8 escolher   BACKSPACE remover   D/N/P ordenar   ENTER encerrar",
             40, 630, 20, GRAY);
}


// --------------------------------------%------------------------------------------------

static void desenharResultado(const Jogo *jogo) {
    DrawText("FIM DA NOITE", 360, 180, 60, MAROON);
    DrawText(TextFormat("Filmes exibidos: %d", quantidadeSelecionada(jogo->sessao)),
             380, 300, 28, DARKGRAY);
    DrawText(TextFormat("Tempo usado: %d de %d min",
                        calcularDuracaoTotal(jogo->sessao), TEMPO_LIMITE),
             380, 345, 28, DARKGRAY);
    DrawText(TextFormat("Pontuacao final: %.1f", calcularPontuacao(jogo->sessao)),
             380, 390, 28, DARKBLUE);
    DrawText("ENTER para voltar ao menu", 400, 500, 22, GRAY);
}



/* ============================== LOOP ============================== */

/* Chamada pelo main: roda o jogo ate a janela ser fechada. */
void iniciarJogo(void) {
    Jogo jogo = {0};              /* zera tudo, inclusive sessao = NULL */
    jogo.telaAtual = TELA_MENU;

    while (!WindowShouldClose()) {
        /* 1. Atualizar: so a tela ativa le o teclado */
        switch (jogo.telaAtual) {
            case TELA_MENU:      atualizarMenu(&jogo);      break;
            case TELA_JOGO:      atualizarPartida(&jogo);   break;
            case TELA_RESULTADO: atualizarResultado(&jogo); break;
        }

        /* 2. Desenhar: so a tela ativa aparece */
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch (jogo.telaAtual) {
            case TELA_MENU:      desenharMenu();            break;
            case TELA_JOGO:      desenharPartida(&jogo);    break;
            case TELA_RESULTADO: desenharResultado(&jogo);  break;
        }
        EndDrawing();
    }

    liberarLista(&jogo.sessao);   /* devolve a memoria dos nos ao sair */
}