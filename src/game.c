
#include <string.h>
#include "raylib.h"
#include "game.h"

#define LARGURA 1100
#define ALTURA 700

/* ============================ CORES ============================ */

static const Color FUNDO = {15, 18, 29, 255};
static const Color PAINEL = {25, 30, 45, 255};
static const Color CARD = {34, 40, 57, 255};
static const Color VERMELHO = {210, 43, 55, 255};
static const Color DOURADO = {232, 184, 75, 255};
static const Color BRANCO = {240, 242, 248, 255};
static const Color CINZA = {155, 164, 180, 255};
static const Color VERDE = {75, 190, 130, 255};

/* ============================ AUXILIARES ============================ */

static void prepararPartida(Jogo *jogo) {
    carregarFilmes(jogo->cartaz, &jogo->quantidadeCartaz);
    liberarLista(&jogo->sessao);
    inicializarLista(&jogo->sessao);
}

static int filmeJaNaSessao(Sessao *inicio, const char *nome) {
    while (inicio != NULL) {
        if (strcmp(inicio->filme.nome, nome) == 0)
            return 1;

        inicio = inicio->prox;
    }

    return 0;
}

/* ============================ ATUALIZAR ============================ */

static void atualizarMenu(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        prepararPartida(jogo);
        jogo->telaAtual = TELA_JOGO;
    }
}

static void atualizarPartida(Jogo *jogo) {
    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            Filme escolhido = jogo->cartaz[i];

            int tempoUsado = calcularDuracaoTotal(jogo->sessao);

            int cabeNoTempo =
                (tempoUsado + escolhido.duracao) <= TEMPO_LIMITE;

            if (cabeNoTempo &&
                !filmeJaNaSessao(jogo->sessao, escolhido.nome)) {
                adicionarFilme(&jogo->sessao, escolhido);
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int ultimo = quantidadeSelecionada(jogo->sessao) - 1;
        removerFilme(&jogo->sessao, ultimo);
    }

    if (IsKeyPressed(KEY_D))
        ordenarPorDuracao(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_N))
        ordenarPorNota(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_P))
        ordenarPorPopularidade(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_ENTER)) {
        jogo->telaAtual = TELA_RESULTADO;
    }
}

static void atualizarResultado(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        jogo->telaAtual = TELA_MENU;
    }
}

/* ============================ COMPONENTES VISUAIS ============================ */

static void desenharPainel(float x, float y,
                           float largura, float altura) {
    Rectangle r = {x, y, largura, altura};

    DrawRectangleRounded(r, 0.08f, 12, PAINEL);
    DrawRectangleLinesEx(r, 1, (Color){55, 63, 82, 255});
}

static void desenharBotao(float x, float y,
                          float largura, float altura,
                          const char *texto, Color cor) {
    Rectangle r = {x, y, largura, altura};

    DrawRectangleRounded(r, 0.2f, 10, cor);

    int tamanho = 18;
    int textoLargura = MeasureText(texto, tamanho);

    DrawText(texto,
             (int)(x + (largura - textoLargura) / 2),
             (int)(y + (altura - tamanho) / 2),
             tamanho, BRANCO);
}

static void desenharBarraTempo(int tempoUsado) {
    float proporcao = (float)tempoUsado / TEMPO_LIMITE;

    if (proporcao > 1.0f)
        proporcao = 1.0f;

    if (proporcao < 0.0f)
        proporcao = 0.0f;

    DrawRectangleRounded(
        (Rectangle){40, 610, 480, 14},
        0.5f, 10, CARD
    );

    if (proporcao > 0) {
        DrawRectangleRounded(
            (Rectangle){40, 610, 480 * proporcao, 14},
            0.5f, 10,
            proporcao >= 0.9f ? VERMELHO : VERDE
        );
    }
}

/* ============================ MENU ============================ */

static void desenharMenu(void) {
    ClearBackground(FUNDO);

    DrawCircle(900, 120, 180, (Color){210, 43, 55, 15});
    DrawCircle(180, 600, 150, (Color){232, 184, 75, 12});

    DrawText("CINERUSH", 380, 185, 85, VERMELHO);

    DrawRectangle(430, 290, 240, 4, DOURADO);

    DrawText("SEU FESTIVAL. SUAS ESCOLHAS.",
             395, 325, 25, BRANCO);

    DrawText("Monte a programacao perfeita para sua noite de cinema.",
             280, 375, 19, CINZA);

    desenharBotao(390, 455, 320, 65,
                  "ENTER - COMECAR", VERMELHO);

    DrawText("ESC - SAIR", 510, 570, 18, CINZA);
}

/* ============================ PARTIDA ============================ */

static void desenharPartida(const Jogo *jogo) {
    ClearBackground(FUNDO);

    /* CABECALHO */

    DrawText("CINERUSH", 40, 22, 34, VERMELHO);
    DrawText("FESTIVAL DE CINEMA", 245, 32, 16, CINZA);

    DrawLine(40, 75, 1060, 75,
             (Color){55, 63, 82, 255});

    /* CARTAZ DE FILMES */

    desenharPainel(30, 95, 665, 485);

    DrawText("CARTAZ DE FILMES", 55, 112, 24, BRANCO);

    DrawText("ESCOLHA OS FILMES PARA SUA SESSAO",
             55, 143, 13, CINZA);

    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        Filme f = jogo->cartaz[i];

        int coluna = i % 2;
        int linha = i / 2;

        float x = 50 + coluna * 315;
        float y = 180 + linha * 92;

        Rectangle card = {x, y, 295, 80};

        DrawRectangleRounded(card, 0.12f, 10, CARD);

        DrawRectangleRounded(
            (Rectangle){x, y, 5, 80},
            0.5f, 8, VERMELHO
        );

        DrawCircle(x + 27, y + 25, 15, VERMELHO);

        DrawText(TextFormat("%d", i + 1),
                 (int)x + 22, (int)y + 16, 18, BRANCO);

        DrawText(f.nome,
                 (int)x + 52, (int)y + 10, 17, BRANCO);

        DrawText(
            TextFormat("%d min  |  Nota %.1f",
                       f.duracao, f.nota),
            (int)x + 52, (int)y + 37, 14, CINZA
        );

        DrawText(
            TextFormat("Popularidade: %d", f.popularidade),
            (int)x + 52, (int)y + 57, 12, DOURADO
        );
    }

    /* SUA PROGRAMACAO */

    desenharPainel(715, 95, 355, 485);

    DrawText("SUA PROGRAMACAO", 740, 112, 22, BRANCO);

    DrawText("FILMES SELECIONADOS",
             740, 143, 13, CINZA);

    int quantidade = quantidadeSelecionada(jogo->sessao);
    int linha = 0;

    if (jogo->sessao == NULL) {
        DrawText("Nenhum filme selecionado.",
                 740, 210, 16, CINZA);

        DrawText("Escolha filmes no cartaz ao lado.",
                 740, 240, 13, CINZA);
    }

    for (Sessao *no = jogo->sessao;
         no != NULL;
         no = no->prox) {

        float y = 180 + linha * 45;

        DrawRectangleRounded(
            (Rectangle){735, y, 315, 38},
            0.15f, 8, CARD
        );

        DrawText(no->filme.nome,
                 748, (int)y + 5, 14, BRANCO);

        DrawText(
            TextFormat("%d min", no->filme.duracao),
            748, (int)y + 22, 11, DOURADO
        );

        linha++;
    }

    /* INDICADORES */

    int tempoUsado = calcularDuracaoTotal(jogo->sessao);
    float pontos = calcularPontuacao(jogo->sessao);

    DrawText("DURACAO DA SESSAO", 40, 590, 14, CINZA);

    DrawText(
        TextFormat("%d / %d MIN", tempoUsado, TEMPO_LIMITE),
        40, 630, 22, BRANCO
    );

    desenharBarraTempo(tempoUsado);

    DrawText("PONTUACAO", 565, 590, 14, CINZA);

    DrawText(TextFormat("%.1f", pontos),
             565, 620, 30, DOURADO);

    DrawText(TextFormat("%d FILMES", quantidade),
             850, 610, 20, BRANCO);

    /* CONTROLES */

    DrawLine(40, 655, 1060, 655,
             (Color){55, 63, 82, 255});

    DrawText("1-8 ESCOLHER", 40, 670, 13, CINZA);
    DrawText("BACKSPACE REMOVER", 225, 670, 13, CINZA);
    DrawText("D / N / P ORDENAR", 480, 670, 13, CINZA);
    DrawText("ENTER FINALIZAR", 790, 670, 13, DOURADO);
}

/* ============================ RESULTADO ============================ */

static void desenharResultado(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 30, VERMELHO);

    DrawLine(40, 75, 1060, 75,
             (Color){55, 63, 82, 255});

    DrawText("FIM DA SESSAO", 360, 135, 45, BRANCO);

    DrawRectangle(390, 195, 320, 4, DOURADO);

    desenharPainel(300, 240, 500, 280);

    DrawText("RESULTADO DO FESTIVAL",
             365, 265, 22, DOURADO);

    DrawText(
        TextFormat("Filmes exibidos: %d",
                   quantidadeSelecionada(jogo->sessao)),
        370, 330, 21, BRANCO
    );

    DrawText(
        TextFormat("Tempo utilizado: %d / %d min",
                   calcularDuracaoTotal(jogo->sessao),
                   TEMPO_LIMITE),
        370, 375, 21, BRANCO
    );

    DrawText("PONTUACAO FINAL",
             370, 430, 15, CINZA);

    DrawText(
        TextFormat("%.1f", calcularPontuacao(jogo->sessao)),
        570, 420, 35, DOURADO
    );

    desenharBotao(390, 550, 320, 55,
                  "ENTER - VOLTAR AO MENU", VERMELHO);
}

/* ============================ LOOP PRINCIPAL ============================ */

void iniciarJogo(void) {
    Jogo jogo = {0};

    jogo.telaAtual = TELA_MENU;

    while (!WindowShouldClose()) {

        /* ATUALIZAR */

        switch (jogo.telaAtual) {
            case TELA_MENU:
                atualizarMenu(&jogo);
                break;

            case TELA_JOGO:
                atualizarPartida(&jogo);
                break;

            case TELA_RESULTADO:
                atualizarResultado(&jogo);
                break;
        }

        /* SAIR */

        if (IsKeyPressed(KEY_ESCAPE))
            break;

        /* DESENHAR */

        BeginDrawing();

        switch (jogo.telaAtual) {
            case TELA_MENU:
                desenharMenu();
                break;

            case TELA_JOGO:
                desenharPartida(&jogo);
                break;

            case TELA_RESULTADO:
                desenharResultado(&jogo);
                break;
        }

        EndDrawing();
    }

    liberarLista(&jogo.sessao);
}
