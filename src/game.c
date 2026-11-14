#include <stdlib.h>
#include <time.h>
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


static const char *nomeDesafio(Desafio desafio) {
    switch (desafio) {
        case NOITE_DOS_CRITICOS:
            return "NOITE DOS CRITICOS";

        case SESSAO_DE_SUCESSO:
            return "SESSAO DE SUCESSO";

        case TEMPO_E_OURO:
            return "TEMPO E OURO";

        case FESTIVAL_DIVERSIFICADO:
            return "FESTIVAL DIVERSIFICADO";

        case MARATONA_CINEMATOGRAFICA:
            return "MARATONA CINEMATOGRAFICA";

        case OBRAS_PRIMAS:
            return "OBRAS-PRIMAS";

        default:
            return "DESAFIO";
    }
}

static const char *descricaoDesafio(Desafio desafio) {
    switch (desafio) {
        case NOITE_DOS_CRITICOS:
            return "Alcance a maior media de notas.";

        case SESSAO_DE_SUCESSO:
            return "Escolha os filmes mais populares.";

        case TEMPO_E_OURO:
            return "Aproxime-se dos 300 minutos.";

        case FESTIVAL_DIVERSIFICADO:
            return "Equilibre notas e popularidade.";

        case MARATONA_CINEMATOGRAFICA:
            return "Selecione o maior numero de filmes.";

        case OBRAS_PRIMAS:
            return "Combine qualidade e popularidade.";

        default:
            return "";
    }
}

static void sortearDesafio(Jogo *jogo) {
    int sorteado;

    do {
        sorteado = rand() % TOTAL_DESAFIOS;
    } while (sorteado == jogo->desafioAnterior);

    jogo->desafioAtual = (Desafio)sorteado;
    jogo->desafioAnterior = sorteado;
}

static float calcularPontuacaoDesafio(const Jogo *jogo) {
    Sessao *atual = jogo->sessao;

    int quantidade = 0;
    int duracao = 0;

    float somaNotas = 0;
    float somaPopularidade = 0;

    while (atual != NULL) {
        quantidade++;
        duracao += atual->filme.duracao;

        somaNotas += atual->filme.nota;
        somaPopularidade += atual->filme.popularidade;

        atual = atual->prox;
    }

    if (quantidade == 0)
        return 0;

    float mediaNotas = somaNotas / quantidade;
    float mediaPopularidade = somaPopularidade / quantidade;

    switch (jogo->desafioAtual) {

        case NOITE_DOS_CRITICOS:
            return (mediaNotas / 10.0f) * 100.0f;

        case SESSAO_DE_SUCESSO:
            return mediaPopularidade;

        case TEMPO_E_OURO:
            return ((float)duracao / TEMPO_LIMITE) * 100.0f;

        case FESTIVAL_DIVERSIFICADO:
            return 100.0f -
                   (float)abs((int)(mediaNotas * 10 - mediaPopularidade));

        case MARATONA_CINEMATOGRAFICA:
            return ((float)quantidade / jogo->quantidadeCartaz) * 100.0f;

        case OBRAS_PRIMAS:
            return ((mediaNotas * 10.0f + mediaPopularidade) / 2.0f);

        default:
            return 0;
    }
}


/* ============================ AUXILIARES ============================ */
static void prepararPartida(Jogo *jogo) {
    carregarFilmes(jogo->cartaz, &jogo->quantidadeCartaz);
    liberarLista(&jogo->sessao);
    inicializarLista(&jogo->sessao);

    sortearDesafio(jogo);
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
    if (IsKeyPressed(KEY_H)) {
        jogo->telaAtual = TELA_DESAFIOS;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        prepararPartida(jogo);
        jogo->telaAtual = TELA_JOGO;
    }
}

static void atualizarDesafios(Jogo *jogo) {
    if (IsKeyPressed(KEY_BACKSPACE) ||
        IsKeyPressed(KEY_ENTER)) {
        jogo->telaAtual = TELA_MENU;
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

    DrawText("H - CONHECER OS DESAFIOS",
         410, 540, 18, DOURADO);

    DrawText("ESC - SAIR", 510, 570, 18, CINZA);
}


static void desenharDesafios(void) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 30, VERMELHO);

    DrawText("GUIA DOS DESAFIOS", 350, 110, 35, BRANCO);

    DrawText("Cada partida possui um objetivo diferente.",
             350, 155, 17, CINZA);

    DrawLine(40, 195, 1060, 195,
             (Color){55, 63, 82, 255});

    /* DESAFIO 1 */
    desenharPainel(55, 220, 475, 115);

    DrawText("1. NOITE DOS CRITICOS",
             75, 235, 20, DOURADO);

    DrawText("Priorize filmes com as maiores notas.",
             75, 270, 16, BRANCO);

    DrawText("Objetivo: conseguir a maior media de notas.",
             75, 300, 14, CINZA);

    /* DESAFIO 2 */
    desenharPainel(570, 220, 475, 115);

    DrawText("2. SESSAO DE SUCESSO",
             590, 235, 20, DOURADO);

    DrawText("Escolha filmes com alta popularidade.",
             590, 270, 16, BRANCO);

    DrawText("Objetivo: conseguir a maior popularidade media.",
             590, 300, 14, CINZA);

    /* DESAFIO 3 */
    desenharPainel(55, 355, 475, 115);

    DrawText("3. TEMPO E OURO",
             75, 370, 20, DOURADO);

    DrawText("Aproveite ao maximo os 300 minutos disponiveis.",
             75, 405, 16, BRANCO);

    DrawText("Objetivo: utilizar o maior tempo possivel.",
             75, 435, 14, CINZA);

    /* DESAFIO 4 */
    desenharPainel(570, 355, 475, 115);

    DrawText("4. FESTIVAL DIVERSIFICADO",
             590, 370, 20, DOURADO);

    DrawText("Equilibre notas e popularidade dos filmes.",
             590, 405, 16, BRANCO);

    DrawText("Objetivo: manter os dois indicadores equilibrados.",
             590, 435, 14, CINZA);

    /* DESAFIO 5 */
    desenharPainel(55, 490, 475, 115);

    DrawText("5. MARATONA CINEMATOGRAFICA",
             75, 505, 19, DOURADO);

    DrawText("Selecione o maior numero de filmes possivel.",
             75, 540, 16, BRANCO);

    DrawText("Objetivo: montar a maior quantidade de filmes.",
             75, 570, 14, CINZA);

    /* DESAFIO 6 */
    desenharPainel(570, 490, 475, 115);

    DrawText("6. OBRAS-PRIMAS",
             590, 505, 20, DOURADO);

    DrawText("Combine qualidade e popularidade.",
             590, 540, 16, BRANCO);

    DrawText("Objetivo: obter uma boa media nos dois indicadores.",
             590, 570, 14, CINZA);

    DrawLine(40, 635, 1060, 635,
             (Color){55, 63, 82, 255});

    DrawText("BACKSPACE - VOLTAR AO MENU",
             390, 660, 16, DOURADO);
}

/* ============================ PARTIDA ============================ */

static void desenharPartida(const Jogo *jogo) {
    ClearBackground(FUNDO);

    /* CABECALHO */

    DrawText("CINERUSH", 40, 22, 34, VERMELHO);
    DrawText("FESTIVAL DE CINEMA", 245, 32, 16, CINZA);

    DrawText("DESAFIO:", 650, 68, 16, DOURADO);

    DrawText(nomeDesafio(jogo->desafioAtual),
            745, 68, 16, BRANCO);

    DrawLine(40, 75, 1060, 75,
             (Color){55, 63, 82, 255});

            
    DrawText(nomeDesafio(jogo->desafioAtual),
         740, 150, 18, DOURADO);

    DrawText(descricaoDesafio(jogo->desafioAtual),
         740, 175, 13, CINZA);
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

    DrawText("FIM DA SESSAO", 360, 115, 45, BRANCO);

    DrawRectangle(390, 175, 320, 4, DOURADO);

    desenharPainel(300, 210, 500, 380);

    DrawText("RESULTADO DO FESTIVAL",
             365, 230, 22, DOURADO);

    DrawText(
        TextFormat("Filmes exibidos: %d",
                   quantidadeSelecionada(jogo->sessao)),
        370, 280, 21, BRANCO
    );

    DrawText(
        TextFormat("Tempo utilizado: %d / %d min",
                   calcularDuracaoTotal(jogo->sessao),
                   TEMPO_LIMITE),
        370, 315, 21, BRANCO
    );

    DrawText("DESAFIO DA PARTIDA",
             370, 365, 15, CINZA);

    DrawText(nomeDesafio(jogo->desafioAtual),
             370, 390, 20, DOURADO);

    DrawText(
        TextFormat("Pontuacao do desafio: %.1f / 100",
                   calcularPontuacaoDesafio(jogo)),
        370, 425, 21, BRANCO
    );

    DrawText("PONTUACAO FINAL",
             370, 470, 15, CINZA);

    DrawText(
        TextFormat("%.1f", calcularPontuacaoDesafio(jogo)),
        570, 460, 35, DOURADO
    );

    desenharBotao(390, 610, 320, 55,
                  "ENTER - VOLTAR AO MENU", VERMELHO);
}

/* ============================ LOOP PRINCIPAL ============================ */

void iniciarJogo(void) {
    srand((unsigned int)time(NULL));

    Jogo jogo = {0};

    jogo.telaAtual = TELA_MENU;
    jogo.desafioAnterior = -1;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1100, 700, "CineRush");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        switch (jogo.telaAtual) {
            case TELA_MENU:
                atualizarMenu(&jogo);
                break;

            case TELA_DESAFIOS:
                atualizarDesafios(&jogo);
                break;

            case TELA_JOGO:
                atualizarPartida(&jogo);
                break;

            case TELA_RESULTADO:
                atualizarResultado(&jogo);
                break;
        }

        BeginDrawing();

        switch (jogo.telaAtual) {
            case TELA_MENU:
                desenharMenu();
                break;

            case TELA_DESAFIOS:
                desenharDesafios();
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
    CloseWindow();
}