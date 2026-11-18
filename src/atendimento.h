#ifndef ATENDIMENTO_H
#define ATENDIMENTO_H

#include "fila.h"
#include "lista.h"

#define DURACAO_EXPEDIENTE 45.0
#define MARGEM_FECHAMENTO 8.0
#define CAPACIDADE_FILA 4
#define META_ATENDIMENTOS 5
#define PONTOS_ACERTO 20.0f

typedef struct {
    FilaClientes fila;
    double tempoRestante;
    double tempoChegada;
    double tempoAviso;
    double intervaloChegada;
    float pacienciaInicial;
    float penalidadeErro;
    float penalidadeFila;
    int nivel;
    int maxIngressos;
    int grupoMinimo;
    int grupoMaximo;
    int proximoId;
    int totalCriados;
    int atendidos;
    int errados;
    int desistentes;
    int recusados;
    float pontos;
    int naoAtendidos;
    int encerrado;
    int filmeSelecionado;
    int ingressosSelecionados;
    int pipocaSelecionada;
    char aviso[120];
} Atendimento;

void inicializarAtendimento(Atendimento *atendimento);
void liberarAtendimento(Atendimento *atendimento);
int iniciarAtendimento(Atendimento *atendimento, const Sessao *sessao, int nivel);
void atualizarAtendimento(Atendimento *atendimento, const Sessao *sessao, float dt);
int atenderCliente(Atendimento *atendimento, const Sessao *sessao);
int metaAtendimentoCumprida(const Atendimento *atendimento);
const Filme *filmeDaProgramacao(const Sessao *sessao, int indice);

#endif