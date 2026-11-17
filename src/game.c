
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "game.h"

static const Color FUNDO = {15, 18, 29, 255};
static const Color PAINEL = {25, 30, 45, 255};
static const Color CARD = {34, 40, 57, 255};
static const Color VERMELHO = {210, 43, 55, 255};
static const Color DOURADO = {232, 184, 75, 255};
static const Color BRANCO = {240, 242, 248, 255};
static const Color CINZA = {155, 164, 180, 255};
static const Color VERDE = {75, 190, 130, 255};

static const char *nomesDesafios[TOTAL_DESAFIOS] = {
    "NOITE DOS CRITICOS",
    "SESSAO DE SUCESSO",
    "TEMPO E OURO",
    "FESTIVAL DIVERSIFICADO",
    "MARATONA CINEMATOGRAFICA",
    "OBRAS-PRIMAS"
};

static const char *descricoesDesafios[TOTAL_DESAFIOS] = {
    "Monte uma sessao com filmes de notas altas.",
    "Escolha filmes com alta popularidade.",
    "Aproveite ao maximo os 300 minutos disponiveis.",
    "Equilibre a qualidade e a popularidade dos filmes.",
    "Selecione a maior quantidade de filmes possivel.",
    "Combine filmes com nota e popularidade elevadas."
};

static void desenharPainel(float x, float y, float largura, float altura) {
    Rectangle r = {x, y, largura, altura};
    DrawRectangleRounded(r, 0.08f, 12, PAINEL);
    DrawRectangleLinesEx(r, 1, (Color){55, 63, 82, 255});
}

static void desenharBotao(float x, float y, float largura, float altura, const char *texto, Color cor) {
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

    DrawRectangleRounded((Rectangle){40, 610, 480, 14}, 0.5f, 10, CARD);

    if (proporcao > 0) {
        DrawRectangleRounded(
            (Rectangle){40, 610, 480 * proporcao, 14},
            0.5f, 10,
            proporcao >= 0.9f ? VERMELHO : VERDE
        );
    }
}

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

static void sortearDesafio(Jogo *jogo) {
    int novoDesafio;

    do {
        novoDesafio = rand() % TOTAL_DESAFIOS;
    } while (TOTAL_DESAFIOS > 1 && novoDesafio == jogo->desafioAnterior);

    jogo->desafioAtual = (Desafio)novoDesafio;
    jogo->desafioAnterior = novoDesafio;
}

static float calcularPontuacaoFinal(const Jogo *jogo) {
    float pontos = calcularPontuacao(jogo->sessao);
    int quantidade = quantidadeSelecionada(jogo->sessao);
    int tempo = calcularDuracaoTotal(jogo->sessao);

    float somaNotas = 0;
    float somaPopularidade = 0;
    int obrasPrimas = 0;

    for (Sessao *no = jogo->sessao; no != NULL; no = no->prox) {
        somaNotas += no->filme.nota;
        somaPopularidade += no->filme.popularidade;

        if (no->filme.nota >= 8.5 && no->filme.popularidade >= 85)
            obrasPrimas++;
    }

    if (quantidade == 0)
        return pontos;

    switch (jogo->desafioAtual) {
        case NOITE_DOS_CRITICOS:
            pontos += (somaNotas / quantidade) * 10;
            break;

        case SESSAO_DE_SUCESSO:
            pontos += (somaPopularidade / quantidade) * 0.5f;
            break;

        case TEMPO_E_OURO:
            pontos += ((float)tempo / TEMPO_LIMITE) * 100;
            break;

        case FESTIVAL_DIVERSIFICADO:
            pontos += ((somaNotas / quantidade) * 5) +
                      ((somaPopularidade / quantidade) * 0.25f);
            break;

        case MARATONA_CINEMATOGRAFICA:
            pontos += quantidade * 20;
            break;

        case OBRAS_PRIMAS:
            pontos += obrasPrimas * 50;
            break;

        default:
            break;
    }

    return pontos;
}

static void atualizarMenu(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        prepararPartida(jogo);
        sortearDesafio(jogo);
        jogo->telaAtual = TELA_JOGO;
    }

    if (IsKeyPressed(KEY_H))
        jogo->telaAtual = TELA_DESAFIOS;

    if (IsKeyPressed(KEY_R))
        jogo->telaAtual = TELA_RANKING;
}

static void atualizarDesafios(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        sortearDesafio(jogo);
        prepararPartida(jogo);
        jogo->telaAtual = TELA_JOGO;
    }

    if (IsKeyPressed(KEY_BACKSPACE))
        jogo->telaAtual = TELA_MENU;
}

static void atualizarPartida(Jogo *jogo) {
    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            Filme escolhido = jogo->cartaz[i];
            int tempoUsado = calcularDuracaoTotal(jogo->sessao);

            if (tempoUsado + escolhido.duracao <= TEMPO_LIMITE &&
                !filmeJaNaSessao(jogo->sessao, escolhido.nome)) {
                adicionarFilme(&jogo->sessao, escolhido);
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int ultimo = quantidadeSelecionada(jogo->sessao) - 1;

        if (ultimo >= 0)
            removerFilme(&jogo->sessao, ultimo);
    }

    if (IsKeyPressed(KEY_D))
        ordenarPorDuracao(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_N))
        ordenarPorNota(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_P))
        ordenarPorPopularidade(jogo->cartaz, jogo->quantidadeCartaz);

    if (IsKeyPressed(KEY_ENTER))
        jogo->telaAtual = TELA_RESULTADO;
}

static void atualizarResultado(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        jogo->nomeJogador[0] = '\0';
        jogo->tamanhoNome = 0;
        jogo->telaAtual = TELA_NOME;
    }
}

static void atualizarNome(Jogo *jogo) {
    int caractere = GetCharPressed();

    while (caractere > 0) {
        if (caractere >= 32 && caractere <= 125 &&
            jogo->tamanhoNome < TAM_NOME - 1) {
            jogo->nomeJogador[jogo->tamanhoNome] = (char)caractere;
            jogo->tamanhoNome++;
            jogo->nomeJogador[jogo->tamanhoNome] = '\0';
        }

        caractere = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && jogo->tamanhoNome > 0) {
        jogo->tamanhoNome--;
        jogo->nomeJogador[jogo->tamanhoNome] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && jogo->tamanhoNome > 0) {
        adicionarRanking(
            jogo->ranking,
            &jogo->quantidadeRanking,
            jogo->nomeJogador,
            calcularPontuacaoFinal(jogo)
        );

        salvarRanking(jogo->ranking, jogo->quantidadeRanking);
        jogo->telaAtual = TELA_RANKING;
    }
}

static void atualizarRanking(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER))
        jogo->telaAtual = TELA_MENU;
}

static void desenharMenu(void) {
    ClearBackground(FUNDO);

    DrawCircle(900, 120, 180, (Color){210, 43, 55, 15});
    DrawCircle(180, 600, 150, (Color){232, 184, 75, 12});

    DrawText("CINERUSH", 380, 185, 85, VERMELHO);
    DrawRectangle(430, 290, 240, 4, DOURADO);

    DrawText("SEU FESTIVAL. SUAS ESCOLHAS.", 395, 325, 25, BRANCO);
    DrawText("Monte a programacao perfeita para sua noite de cinema.",
             280, 375, 19, CINZA);

    desenharBotao(390, 455, 320, 65, "ENTER - COMECAR", VERMELHO);

    DrawText("H - DESAFIOS", 420, 550, 18, DOURADO);
    DrawText("R - RANKING", 590, 550, 18, DOURADO);
    DrawText("ESC - SAIR", 510, 610, 18, CINZA);
}

static void desenharDesafios(void) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 34, VERMELHO);
    DrawText("DESAFIOS DO FESTIVAL", 350, 120, 35, BRANCO);

    desenharPainel(220, 210, 660, 270);

    for (int i = 0; i < TOTAL_DESAFIOS; i++) {
        DrawText(nomesDesafios[i], 270, 235 + i * 35, 18, DOURADO);
        DrawText(descricoesDesafios[i], 270, 258 + i * 35, 14, CINZA);
    }

    desenharBotao(390, 535, 320, 55, "ENTER - JOGAR", VERMELHO);
    DrawText("BACKSPACE - VOLTAR", 430, 620, 17, CINZA);
}

static void desenharPartida(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 22, 34, VERMELHO);
    DrawText("FESTIVAL DE CINEMA", 245, 32, 16, CINZA);

    DrawLine(40, 75, 1060, 75, (Color){55, 63, 82, 255});

    DrawText(nomesDesafios[jogo->desafioAtual], 40, 85, 18, DOURADO);
    DrawText(descricoesDesafios[jogo->desafioAtual], 40, 108, 13, CINZA);

    desenharPainel(30, 135, 665, 445);

    DrawText("CARTAZ DE FILMES", 55, 150, 24, BRANCO);
    DrawText("ESCOLHA OS FILMES PARA SUA SESSAO", 55, 180, 13, CINZA);

    for (int i = 0; i < jogo->quantidadeCartaz; i++) {
        Filme f = jogo->cartaz[i];

        int coluna = i % 2;
        int linha = i / 2;

        float x = 50 + coluna * 315;
        float y = 210 + linha * 78;

        DrawRectangleRounded((Rectangle){x, y, 295, 68}, 0.12f, 10, CARD);
        DrawRectangleRounded((Rectangle){x, y, 5, 68}, 0.5f, 8, VERMELHO);
        DrawCircle(x + 27, y + 25, 15, VERMELHO);

        DrawText(TextFormat("%d", i + 1), (int)x + 22, (int)y + 16, 18, BRANCO);
        DrawText(f.nome, (int)x + 52, (int)y + 8, 17, BRANCO);

        DrawText(TextFormat("%d min  |  Nota %.1f", f.duracao, f.nota),
                 (int)x + 52, (int)y + 32, 14, CINZA);

        DrawText(TextFormat("Popularidade: %d", f.popularidade),
                 (int)x + 52, (int)y + 49, 12, DOURADO);
    }

    desenharPainel(715, 135, 355, 445);

    DrawText("SUA PROGRAMACAO", 740, 150, 22, BRANCO);
    DrawText("FILMES SELECIONADOS", 740, 180, 13, CINZA);

    int quantidade = quantidadeSelecionada(jogo->sessao);
    int linha = 0;

    if (jogo->sessao == NULL) {
        DrawText("Nenhum filme selecionado.", 740, 240, 16, CINZA);
    }

    for (Sessao *no = jogo->sessao; no != NULL; no = no->prox) {
        float y = 210 + linha * 42;

        DrawRectangleRounded((Rectangle){735, y, 315, 36}, 0.15f, 8, CARD);
        DrawText(no->filme.nome, 748, (int)y + 5, 14, BRANCO);
        DrawText(TextFormat("%d min", no->filme.duracao),
                 748, (int)y + 21, 11, DOURADO);

        linha++;
    }

    int tempoUsado = calcularDuracaoTotal(jogo->sessao);
    float pontos = calcularPontuacao(jogo->sessao);

    DrawText("DURACAO DA SESSAO", 40, 590, 14, CINZA);
    DrawText(TextFormat("%d / %d MIN", tempoUsado, TEMPO_LIMITE),
             40, 630, 22, BRANCO);

    desenharBarraTempo(tempoUsado);

    DrawText("PONTUACAO", 565, 590, 14, CINZA);
    DrawText(TextFormat("%.1f", pontos), 565, 620, 30, DOURADO);
    DrawText(TextFormat("%d FILMES", quantidade), 850, 610, 20, BRANCO);

    DrawLine(40, 655, 1060, 655, (Color){55, 63, 82, 255});

    DrawText("1-8 ESCOLHER", 40, 670, 13, CINZA);
    DrawText("BACKSPACE REMOVER", 225, 670, 13, CINZA);
    DrawText("D / N / P ORDENAR", 480, 670, 13, CINZA);
    DrawText("ENTER FINALIZAR", 790, 670, 13, DOURADO);
}

static void desenharResultado(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 30, VERMELHO);
    DrawLine(40, 75, 1060, 75, (Color){55, 63, 82, 255});

    DrawText("FIM DA SESSAO", 360, 115, 45, BRANCO);
    DrawRectangle(390, 175, 320, 4, DOURADO);

    desenharPainel(300, 210, 500, 350);

    DrawText("RESULTADO DO FESTIVAL", 365, 235, 22, DOURADO);
    DrawText(nomesDesafios[jogo->desafioAtual], 365, 280, 17, BRANCO);

    DrawText(TextFormat("Filmes exibidos: %d",
                        quantidadeSelecionada(jogo->sessao)),
             370, 340, 21, BRANCO);

    DrawText(TextFormat("Tempo utilizado: %d / %d min",
                        calcularDuracaoTotal(jogo->sessao), TEMPO_LIMITE),
             370, 385, 21, BRANCO);

    DrawText("PONTUACAO FINAL", 370, 440, 15, CINZA);

    DrawText(TextFormat("%.1f", calcularPontuacaoFinal(jogo)),
             570, 430, 35, DOURADO);

    desenharBotao(390, 590, 320, 55, "ENTER - CONTINUAR", VERMELHO);
}

static void desenharNome(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 34, VERMELHO);
    DrawText("REGISTRAR PONTUACAO", 300, 160, 35, BRANCO);

    desenharPainel(300, 250, 500, 190);

    DrawText("Digite seu nome:", 350, 285, 20, CINZA);

    DrawRectangleRounded((Rectangle){350, 330, 400, 50}, 0.1f, 8, CARD);
    DrawText(jogo->nomeJogador, 365, 343, 22, BRANCO);

    DrawText("ENTER - CONFIRMAR", 420, 490, 18, DOURADO);
    DrawText("BACKSPACE - APAGAR", 420, 525, 15, CINZA);
}

static void desenharTelaRanking(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 25, 34, VERMELHO);
    DrawText("RANKING", 430, 100, 45, DOURADO);

    desenharPainel(250, 175, 600, 420);

    DrawText("POS.", 290, 200, 16, CINZA);
    DrawText("JOGADOR", 390, 200, 16, CINZA);
    DrawText("PONTUACAO", 700, 200, 16, CINZA);

    for (int i = 0; i < jogo->quantidadeRanking; i++) {
        int y = 245 + i * 32;

        DrawText(TextFormat("%d", i + 1), 300, y, 18,
                 i == 0 ? DOURADO : BRANCO);

        DrawText(jogo->ranking[i].nome, 390, y, 18, BRANCO);

        DrawText(TextFormat("%.1f", jogo->ranking[i].pontuacao),
                 700, y, 18, DOURADO);
    }

    if (jogo->quantidadeRanking == 0)
        DrawText("Nenhuma pontuacao registrada.", 390, 270, 18, CINZA);

    DrawText("ENTER - VOLTAR AO MENU", 410, 635, 18, CINZA);
}

void iniciarJogo(void) {
    Jogo jogo = {0};

    InitWindow(1100, 700, "CineRush");
    SetTargetFPS(60);

    jogo.telaAtual = TELA_MENU;
    jogo.desafioAnterior = -1;

    srand((unsigned int)time(NULL));
    carregarRanking(jogo.ranking, &jogo.quantidadeRanking);

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
            case TELA_NOME:
                atualizarNome(&jogo);
                break;
            case TELA_RANKING:
                atualizarRanking(&jogo);
                break;
        }

        if (IsKeyPressed(KEY_ESCAPE))
            break;

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
            case TELA_NOME:
                desenharNome(&jogo);
                break;
            case TELA_RANKING:
                desenharTelaRanking(&jogo);
                break;
        }

        EndDrawing();
    }

    liberarLista(&jogo.sessao);
    CloseWindow();
}
