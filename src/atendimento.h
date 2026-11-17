#ifndef ATENDIMENTO_H
#define ATENDIMENTO_H

#include "fila.h"
#include "lista.h"

#define DURACAO_EXPEDIENTE 45.0f
#define INTERVALO_CHEGADA 5.0f
#define PACIENCIA_CLIENTE 10.0f

typedef struct {
    FilaClientes fila;
    double tempoRestante;
    double tempoChegada;
    double tempoAviso;
    int proximoId;
    int totalCriados;
    int atendidos;
    int naoAtendidos;
    int encerrado;
    char aviso[120];
} Atendimento;

void inicializarAtendimento(Atendimento *atendimento);
void liberarAtendimento(Atendimento *atendimento);
int iniciarAtendimento(Atendimento *atendimento, const Sessao *sessao);
void atualizarAtendimento(Atendimento *atendimento, const Sessao *sessao, float dt);
int atenderCliente(Atendimento *atendimento);

#endif
