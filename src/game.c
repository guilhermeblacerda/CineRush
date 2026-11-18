#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "game.h"

#define LARGURA 1100
#define ALTURA 700

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
    "Priorize filmes com notas elevadas.",
    "Escolha filmes com alta popularidade.",
    "Aproveite o tempo disponivel.",
    "Equilibre nota e popularidade.",
    "Selecione a maior quantidade de filmes.",
    "Combine filmes de alta nota e popularidade."
};

static void desenharPainel(float x, float y, float largura, float altura) {
    Rectangle r = {x, y, largura, altura};
    DrawRectangleRounded(r, 0.08f, 12, PAINEL);
    DrawRectangleLinesEx(r, 1, (Color){55, 63, 82, 255});
}

static void desenharBotao(float x, float y, float largura, float altura,
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

static void configurarDificuldade(Jogo *jogo) {
    if (jogo->nivel <= 1) {
        jogo->nivel = 1;
        jogo->tempoLimite = 400;
        jogo->objetivoFilmes = 2;
        jogo->pontuacaoMinima = 100.0f;
    } else if (jogo->nivel == 2) {
        jogo->tempoLimite = 370;
        jogo->objetivoFilmes = 3;
        jogo->pontuacaoMinima = 180.0f;
    } else {
        jogo->nivel = 3;
        jogo->tempoLimite = 350;
        jogo->objetivoFilmes = 4;
        jogo->pontuacaoMinima = 260.0f;
    }
}

static void prepararPartida(Jogo *jogo) {
    configurarDificuldade(jogo);
    carregarFilmes(jogo->cartaz, &jogo->quantidadeCartaz);
    liberarLista(&jogo->sessao);
    inicializarLista(&jogo->sessao);
    jogo->partidaVencida = 0;
    liberarAtendimento(&jogo->atendimento);
    inicializarAtendimento(&jogo->atendimento);
    jogo->avisoProgramacao[0] = '\0';
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
    int novo;

    do {
        novo = rand() % TOTAL_DESAFIOS;
    } while (TOTAL_DESAFIOS > 1 && novo == jogo->desafioAnterior);

    jogo->desafioAtual = (Desafio)novo;
    jogo->desafioAnterior = novo;
}

static float calcularPontuacaoProgramacao(const Jogo *jogo) {
    int quantidade = quantidadeSelecionada(jogo->sessao);
    float pontuacao = calcularPontuacao(jogo->sessao);

    if (quantidade == 0)
        return 0.0f;

    float somaNotas = 0.0f;
    float somaPopularidade = 0.0f;
    int obrasPrimas = 0;

    for (Sessao *no = jogo->sessao; no != NULL; no = no->prox) {
        somaNotas += no->filme.nota;
        somaPopularidade += no->filme.popularidade;

        if (no->filme.nota >= 8.5f &&
            no->filme.popularidade >= 85) {
            obrasPrimas++;
        }
    }

    float mediaNota = somaNotas / quantidade;
    float mediaPopularidade = somaPopularidade / quantidade;
    int tempo = calcularDuracaoTotal(jogo->sessao);

    switch (jogo->desafioAtual) {
        case NOITE_DOS_CRITICOS:
            pontuacao += mediaNota * 10.0f;
            break;

        case SESSAO_DE_SUCESSO:
            pontuacao += mediaPopularidade * 0.5f;
            break;

        case TEMPO_E_OURO:
            pontuacao += ((float)tempo / jogo->tempoLimite) * 100.0f;
            break;

        case FESTIVAL_DIVERSIFICADO:
            pontuacao += mediaNota * 5.0f +
                         mediaPopularidade * 0.25f;
            break;

        case MARATONA_CINEMATOGRAFICA:
            pontuacao += quantidade * 20.0f;
            break;

        case OBRAS_PRIMAS:
            pontuacao += obrasPrimas * 50.0f;
            break;

        default:
            break;
    }

    return pontuacao;
}

static float calcularPontuacaoFinal(const Jogo *jogo) {
    float pontos = calcularPontuacaoProgramacao(jogo) + jogo->atendimento.pontos;
    return pontos > 0.0f ? pontos : 0.0f;
}

static int verificarVitoria(const Jogo *jogo) {
    int quantidade = quantidadeSelecionada(jogo->sessao);
    float pontuacao = calcularPontuacaoFinal(jogo);

    return quantidade >= jogo->objetivoFilmes &&
           pontuacao >= jogo->pontuacaoMinima &&
           metaAtendimentoCumprida(&jogo->atendimento);
}

static void atualizarMenu(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER)) {
        sortearDesafio(jogo);
        prepararPartida(jogo);
        jogo->telaAtual = TELA_JOGO;
    }

    if (IsKeyPressed(KEY_H))
        jogo->telaAtual = TELA_DESAFIOS;

    if (IsKeyPressed(KEY_R))
        jogo->telaAtual = TELA_RANKING;
}

static void atualizarDesafios(Jogo *jogo) {
    for (int i = 0; i < TOTAL_DESAFIOS; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            jogo->desafioAtual = (Desafio)i;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        jogo->desafioAnterior = jogo->desafioAtual;
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

            int cabeNoTempo =
                tempoUsado + escolhido.duracao <= jogo->tempoLimite;

            if (cabeNoTempo &&
                !filmeJaNaSessao(jogo->sessao, escolhido.nome)) {
                if (adicionarFilme(&jogo->sessao, escolhido))
                    jogo->avisoProgramacao[0] = '\0';
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

    if (IsKeyPressed(KEY_ENTER)) {
        if (jogo->sessao == NULL) {
            strcpy(jogo->avisoProgramacao, "Selecione pelo menos um filme.");
        } else if (iniciarAtendimento(&jogo->atendimento, jogo->sessao, jogo->nivel)) {
            jogo->telaAtual = TELA_ATENDIMENTO;
        } else {
            strcpy(jogo->avisoProgramacao, "Nao foi possivel iniciar o atendimento.");
        }
    }
}

static void atualizarTelaAtendimento(Jogo *jogo) {
    Atendimento *atendimento = &jogo->atendimento;
    const Cliente *anterior = primeiroCliente(&atendimento->fila);
    int idAnterior = anterior != NULL ? anterior->id : -1;

    atualizarAtendimento(atendimento, jogo->sessao, GetFrameTime());

    if (atendimento->encerrado) {
        jogo->partidaVencida = verificarVitoria(jogo);
        jogo->telaAtual = TELA_RESULTADO;
        return;
    }

    const Cliente *atual = primeiroCliente(&atendimento->fila);

    if (atual == NULL || atual->id != idAnterior)
        return;

    for (int i = 0; i < MAX_FILMES; i++) {
        if (IsKeyPressed(KEY_ONE + i) &&
            filmeDaProgramacao(jogo->sessao, i)) {
            atendimento->filmeSelecionado = i;
        }
    }

    if (IsKeyPressed(KEY_RIGHT) &&
        atendimento->ingressosSelecionados < atendimento->maxIngressos)
        atendimento->ingressosSelecionados++;

    if (IsKeyPressed(KEY_LEFT) && atendimento->ingressosSelecionados > 1)
        atendimento->ingressosSelecionados--;

    if (IsKeyPressed(KEY_P))
        atendimento->pipocaSelecionada = !atendimento->pipocaSelecionada;

    if (IsKeyPressed(KEY_SPACE))
        atenderCliente(atendimento, jogo->sessao);
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
        if (caractere >= 32 &&
            caractere <= 125 &&
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

    if (IsKeyPressed(KEY_ENTER)) {
        if (jogo->tamanhoNome == 0) {
            strcpy(jogo->nomeJogador, "Jogador");
        }

        adicionarRanking(
            jogo->ranking,
            &jogo->quantidadeRanking,
            jogo->nomeJogador,
            calcularPontuacaoFinal(jogo)
        );

        salvarRanking(jogo->ranking, jogo->quantidadeRanking);

        if (jogo->partidaVencida && jogo->nivel < 3)
            jogo->nivel++;

        jogo->telaAtual = TELA_MENU;
    }
}

static void atualizarRanking(Jogo *jogo) {
    if (IsKeyPressed(KEY_ENTER) ||
        IsKeyPressed(KEY_BACKSPACE)) {
        jogo->telaAtual = TELA_MENU;
    }
}

static void desenharBarraTempo(int tempoUsado, int tempoLimite) {
    float proporcao = (float)tempoUsado / tempoLimite;

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

static void desenharMenu(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawCircle(900, 120, 180, (Color){210, 43, 55, 15});
    DrawCircle(180, 600, 150, (Color){232, 184, 75, 12});

    DrawText("CINERUSH", 380, 150, 85, VERMELHO);
    DrawRectangle(430, 255, 240, 4, DOURADO);

    DrawText("SEU FESTIVAL. SUAS ESCOLHAS.",
             395, 290, 25, BRANCO);

    DrawText("Monte a programacao perfeita para sua noite de cinema.",
             280, 340, 19, CINZA);

    DrawText(
        TextFormat("NIVEL ATUAL: %d", jogo->nivel),
        470, 405, 22, DOURADO
    );

    desenharBotao(390, 450, 320, 55,
                  "ENTER - COMECAR", VERMELHO);

    DrawText("H - DESAFIOS", 300, 550, 17, CINZA);
    DrawText("R - RANKING", 490, 550, 17, CINZA);
    DrawText("ESC - SAIR", 680, 550, 17, CINZA);
}

static void desenharDesafios(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("DESAFIOS", 420, 35, 42, VERMELHO);
    DrawText("ESCOLHA UM DESAFIO PARA SUA SESSAO",
             330, 90, 17, CINZA);

    for (int i = 0; i < TOTAL_DESAFIOS; i++) {
        int coluna = i % 2;
        int linha = i / 2;

        float x = 130 + coluna * 440;
        float y = 155 + linha * 135;

        Color cor = jogo->desafioAtual == (Desafio)i ? VERMELHO : CARD;

        DrawRectangleRounded(
            (Rectangle){x, y, 400, 105},
            0.12f, 10, cor
        );

        DrawText(TextFormat("%d - %s", i + 1, nomesDesafios[i]),
                 (int)x + 20, (int)y + 18, 18, BRANCO);

        DrawText(descricoesDesafios[i],
                 (int)x + 20, (int)y + 55, 14, BRANCO);
    }

    DrawText("1-6 SELECIONAR     ENTER CONFIRMAR     BACKSPACE VOLTAR",
             180, 615, 17, CINZA);
}

static void desenharPartida(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("CINERUSH", 40, 22, 34, VERMELHO);
    DrawText("FESTIVAL DE CINEMA", 245, 32, 16, CINZA);

    DrawText(
    TextFormat("DESAFIO: %s", nomesDesafios[jogo->desafioAtual]),
    245, 53, 14, DOURADO
    );

    DrawText(
        TextFormat("NIVEL %d", jogo->nivel),
        850, 25, 20, DOURADO
    );

    DrawLine(40, 75, 1060, 75, (Color){55, 63, 82, 255});

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
            TextFormat("%d min | Nota %.1f",
                       f.duracao, f.nota),
            (int)x + 52, (int)y + 37, 14, CINZA
        );

        DrawText(
            TextFormat("Popularidade: %d", f.popularidade),
            (int)x + 52, (int)y + 57, 12, DOURADO
        );
    }

    desenharPainel(715, 95, 355, 485);

    DrawText("SUA PROGRAMACAO", 740, 112, 22, BRANCO);
    DrawText("FILMES SELECIONADOS", 740, 143, 13, CINZA);

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

    int tempoUsado = calcularDuracaoTotal(jogo->sessao);
    float pontos = calcularPontuacaoFinal(jogo);

    DrawText("DURACAO DA SESSAO", 40, 590, 14, CINZA);

    DrawText(
        TextFormat("%d / %d MIN", tempoUsado, jogo->tempoLimite),
        40, 630, 22, BRANCO
    );

    desenharBarraTempo(tempoUsado, jogo->tempoLimite);

    DrawText("PONTUACAO", 565, 590, 14, CINZA);
    DrawText(TextFormat("%.1f", pontos), 565, 620, 30, DOURADO);

    DrawText(TextFormat("%d FILMES", quantidade),
             850, 610, 20, BRANCO);

    DrawText(
        TextFormat("META: %d FILMES | %.0f PONTOS",
                   jogo->objetivoFilmes,
                   jogo->pontuacaoMinima),
        735, 535, 13, DOURADO
    );

    DrawLine(40, 655, 1060, 655, (Color){55, 63, 82, 255});

    DrawText("1-8 ESCOLHER", 40, 670, 13, CINZA);
    DrawText("BACKSPACE REMOVER", 225, 670, 13, CINZA);
    DrawText("D / N / P ORDENAR", 480, 670, 13, CINZA);
    DrawText("ENTER ABRIR CINEMA", 790, 670, 13, DOURADO);
    if (jogo->avisoProgramacao[0] != '\0')
        DrawText(jogo->avisoProgramacao, 735, 560, 12, VERMELHO);
    else
        DrawText(TextFormat("DEPOIS: ACERTE %d ATENDIMENTOS", META_ATENDIMENTOS),
                 735, 560, 12, CINZA);
}


static void desenharTextoAjustado(const char *texto, int x, int y,
                                int tamanho, int largura, Color cor) {
    while (tamanho > 10 && MeasureText(texto, tamanho) > largura)
        tamanho--;
    DrawText(texto, x, y, tamanho, cor);
}

static void desenharCinemaEstatico(void) {
    desenharPainel(30, 95, 400, 460);
    DrawText("CINEMA ABERTO", 112, 115, 24, DOURADO);
    DrawRectangleRounded((Rectangle){65, 165, 330, 160}, 0.05f, 8, BRANCO);
    DrawRectangle(65, 165, 18, 160, VERMELHO);
    DrawRectangle(377, 165, 18, 160, VERMELHO);
    DrawText("CINERUSH", 137, 213, 31, VERMELHO);
    DrawText("SESSAO EM CARTAZ", 131, 262, 17, PAINEL);

    for (int linha = 0; linha < 3; linha++) {
        for (int coluna = 0; coluna < 6; coluna++) {
            float x = 65 + coluna * 55;
            float y = 352 + linha * 47;
            DrawRectangleRounded((Rectangle){x, y, 40, 28}, 0.25f, 8, VERMELHO);
            DrawRectangle((int)x - 3, (int)y + 22, 46, 8, DOURADO);
        }
    }

    DrawText("BILHETERIA + PIPOCA", 114, 510, 19, BRANCO);
}

static void desenharFilaEspera(const Atendimento *atendimento) {
    desenharPainel(450, 550, 620, 85);

    DrawText(TextFormat("FILA: %d CLIENTES (INCLUI O BALCAO)",
                        atendimento->fila.quantidade),
             470, 558, 16, DOURADO);

    const NoCliente *no = atendimento->fila.inicio;

    if (no != NULL)
        no = no->prox;

    if (no == NULL)
        DrawText("Ninguem aguardando na fila.", 470, 590, 14, CINZA);

    for (int i = 0; no != NULL && i < 3; i++, no = no->prox) {
        int y = 580 + i * 17;

        desenharTextoAjustado(
            TextFormat("#%02d  %s", no->cliente.id,
                       no->cliente.filmeDesejado.nome),
            470, y, 13, 365, CINZA
        );

        DrawText(TextFormat("%d ING. | %s PIPOCA",
                            no->cliente.quantidadeIngressos,
                            no->cliente.querPipoca ? "COM" : "SEM"),
                 855, y, 12, BRANCO);
    }
}

static void desenharTelaAtendimento(const Jogo *jogo) {
    const Atendimento *atendimento = &jogo->atendimento;
    const Cliente *cliente = primeiroCliente(&atendimento->fila);

    ClearBackground(FUNDO);
    DrawText("CINERUSH", 40, 22, 34, VERMELHO);
    DrawText(TextFormat("ATENDIMENTO | NIVEL %d", atendimento->nivel),
             245, 32, 18, BRANCO);
    DrawText(TextFormat("FECHA EM %.1f s", atendimento->tempoRestante),
             815, 25, 23, DOURADO);

    double proxima = atendimento->intervaloChegada - atendimento->tempoChegada;

    DrawText(
        atendimento->tempoRestante - proxima > MARGEM_FECHAMENTO
        ? TextFormat("Proxima chegada: %.1f s", proxima)
        : "Entradas encerradas",
        815, 55, 14, CINZA
    );

    DrawLine(40, 80, 1060, 80, CINZA);
    desenharCinemaEstatico();
    desenharPainel(450, 95, 620, 150);

    if (cliente != NULL) {
        DrawText(TextFormat("PEDIDO DO CLIENTE #%02d", cliente->id),
                 470, 110, 20, DOURADO);

        desenharTextoAjustado(cliente->filmeDesejado.nome,
                             470, 143, 24, 575, BRANCO);

        DrawText(TextFormat("%d INGRESSO(S) | %s PIPOCA",
                            cliente->quantidadeIngressos,
                            cliente->querPipoca ? "COM" : "SEM"),
                 470, 178, 20, DOURADO);

        float proporcao = cliente->paciencia / atendimento->pacienciaInicial;

        if (proporcao < 0.0f)
            proporcao = 0.0f;
        if (proporcao > 1.0f)
            proporcao = 1.0f;

        DrawText(TextFormat("PACIENCIA: %.1f s", cliente->paciencia),
                 470, 209, 13, CINZA);
        DrawRectangle(470, 230, 575, 6, CARD);
        DrawRectangle(470, 230, (int)(575 * proporcao), 6,
                      proporcao > 0.3f ? VERDE : VERMELHO);
    } else {
        DrawText("Aguardando o proximo cliente...",
                 470, 145, 23, BRANCO);
    }

    desenharPainel(450, 260, 620, 205);
    DrawText("MONTE O PEDIDO: USE 1-8 PARA ESCOLHER O FILME",
             470, 274, 16, BRANCO);

    const Sessao *no = jogo->sessao;

    for (int i = 0; no != NULL && i < MAX_FILMES; i++, no = no->prox) {
        int x = 470 + (i % 2) * 295;
        int y = 305 + (i / 2) * 35;
        int selecionado = atendimento->filmeSelecionado == i;

        DrawRectangle(x, y, 280, 29, selecionado ? VERMELHO : CARD);

        desenharTextoAjustado(
            TextFormat("%d - %s", i + 1, no->filme.nome),
            x + 8, y + 6, 16, 264, BRANCO
        );
    }

    desenharPainel(450, 478, 620, 60);

    DrawText(TextFormat("INGRESSOS: %d (ESQ/DIR)",
                        atendimento->ingressosSelecionados),
             470, 489, 18, BRANCO);

    DrawText(TextFormat("PIPOCA: %s (P)",
                        atendimento->pipocaSelecionada ? "SIM" : "NAO"),
             790, 489, 18, BRANCO);

    DrawText(
        atendimento->filmeSelecionado < 0
        ? "Escolha um filme antes de entregar."
        : "ESPACO - ENTREGAR PEDIDO",
        470, 515, 15, DOURADO
    );

    DrawText(TextFormat("ACERTOS: %d | META: %d",
                        atendimento->atendidos, META_ATENDIMENTOS),
             45, 574, 20, VERDE);

    DrawText(TextFormat("ERROS: %d | SAIRAM: %d | RECUSADOS: %d",
                        atendimento->errados, atendimento->desistentes,
                        atendimento->recusados),
             45, 600, 13, CINZA);

    DrawText(TextFormat("SALDO: %+.0f PONTOS", atendimento->pontos),
             45, 621, 16, DOURADO);

    desenharFilaEspera(atendimento);
    DrawLine(40, 641, 1060, 641, CINZA);

    if (atendimento->tempoAviso > 0.0f) {
        desenharTextoAjustado(atendimento->aviso,
                             45, 652, 18, 1010, DOURADO);
    }

    DrawText(
        TextFormat(
            "SO O PRIMEIRO PERDE PACIENCIA | ERRO / DESISTENCIA / AO FECHAR: -%.0f | FILA CHEIA: -%.0f",
            atendimento->penalidadeErro, atendimento->penalidadeFila
        ),
        45, 681, 13, CINZA
    );
}

static void desenharResultado(const Jogo *jogo) {
    const Atendimento *atendimento = &jogo->atendimento;
    ClearBackground(FUNDO);
    DrawText("CINERUSH", 40, 25, 30, VERMELHO);
    DrawLine(40, 75, 1060, 75, CINZA);
    DrawText("FIM DO EXPEDIENTE", 315, 104, 39, BRANCO);
    desenharPainel(210, 168, 680, 415);

    DrawText(TextFormat("NIVEL %d | %s", jogo->nivel,
                        nomesDesafios[jogo->desafioAtual]),
             240, 190, 18, DOURADO);
    DrawText(TextFormat("Programacao: %d filmes | %d / %d minutos",
                        quantidadeSelecionada(jogo->sessao),
                        calcularDuracaoTotal(jogo->sessao), jogo->tempoLimite),
             240, 228, 19, BRANCO);
    DrawText(TextFormat("Pontos dos filmes e desafio: %.1f",
                        calcularPontuacaoProgramacao(jogo)),
             240, 265, 20, BRANCO);
    DrawText(TextFormat("Acertos: %d | Erros: %d | Desistencias: %d",
                        atendimento->atendidos, atendimento->errados,
                        atendimento->desistentes),
             240, 305, 19, BRANCO);
    DrawText(TextFormat("Ao fechar: %d | Recusados: %d",
                        atendimento->naoAtendidos, atendimento->recusados),
             240, 338, 18, CINZA);
    DrawText(TextFormat("Saldo do atendimento: %+.0f pontos", atendimento->pontos),
             240, 372, 20, DOURADO);
    DrawText(TextFormat("TOTAL FINAL: %.1f", calcularPontuacaoFinal(jogo)),
             240, 415, 30, DOURADO);
    DrawText(TextFormat("METAS: %d filmes | %.0f pontos | %d atendimentos corretos",
                        jogo->objetivoFilmes, jogo->pontuacaoMinima,
                        META_ATENDIMENTOS),
             240, 463, 16, CINZA);
    DrawText(jogo->partidaVencida ? "DESAFIO CONCLUIDO!" : "OBJETIVO NAO ALCANCADO",
             240, 511, 25, jogo->partidaVencida ? VERDE : VERMELHO);
    desenharBotao(390, 610, 320, 50, "ENTER - CONTINUAR", VERMELHO);
}

static void desenharNome(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("REGISTRAR RESULTADO", 300, 150, 40, BRANCO);
    DrawText("Digite seu nome para o ranking:",
             350, 260, 20, CINZA);

    DrawRectangleRounded(
        (Rectangle){300, 310, 500, 65},
        0.1f, 10, CARD
    );

    DrawText(jogo->nomeJogador, 325, 330, 25, BRANCO);

    DrawText("ENTER - CONFIRMAR",
             440, 430, 18, DOURADO);

    DrawText("BACKSPACE - APAGAR",
             440, 465, 15, CINZA);
}

static void desenharTelaRanking(const Jogo *jogo) {
    ClearBackground(FUNDO);

    DrawText("RANKING", 430, 35, 42, VERMELHO);

    DrawText("TOP 10 JOGADORES",
             405, 95, 18, CINZA);

    for (int i = 0; i < jogo->quantidadeRanking; i++) {
        float y = 145 + i * 43;

        DrawRectangleRounded(
            (Rectangle){250, y, 600, 36},
            0.15f, 8, CARD
        );

        DrawText(
            TextFormat("%d.", i + 1),
            270, (int)y + 8, 17, DOURADO
        );

        DrawText(
            jogo->ranking[i].nome,
            330, (int)y + 8, 17, BRANCO
        );

        DrawText(
            TextFormat("%.1f", jogo->ranking[i].pontuacao),
            720, (int)y + 8, 17, VERDE
        );
    }

    if (jogo->quantidadeRanking == 0) {
        DrawText("Ainda nao existem pontuacoes registradas.",
                 310, 300, 19, CINZA);
    }

    DrawText("ENTER OU BACKSPACE - VOLTAR",
             365, 625, 18, CINZA);
}

void iniciarJogo(void) {
    Jogo jogo = {0};

    InitWindow(LARGURA, ALTURA, "CineRush");
    SetTargetFPS(60);

    srand((unsigned int)time(NULL));

    jogo.telaAtual = TELA_MENU;
    jogo.nivel = 1;
    jogo.desafioAnterior = -1;
    jogo.desafioAtual = NOITE_DOS_CRITICOS;

    inicializarAtendimento(&jogo.atendimento);
    configurarDificuldade(&jogo);
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

            case TELA_ATENDIMENTO:
                atualizarTelaAtendimento(&jogo);
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

            default:
                jogo.telaAtual = TELA_MENU;
                break;
        }

        if (IsKeyPressed(KEY_ESCAPE))
            break;

        BeginDrawing();

        switch (jogo.telaAtual) {
            case TELA_MENU:
                desenharMenu(&jogo);
                break;

            case TELA_DESAFIOS:
                desenharDesafios(&jogo);
                break;

            case TELA_JOGO:
                desenharPartida(&jogo);
                break;

            case TELA_ATENDIMENTO:
                desenharTelaAtendimento(&jogo);
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

            default:
                ClearBackground(FUNDO);
                break;
        }

        EndDrawing();
    }

    liberarAtendimento(&jogo.atendimento);
    liberarLista(&jogo.sessao);
    CloseWindow();
}
