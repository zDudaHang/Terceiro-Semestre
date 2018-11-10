#ifndef __COZINHA_H__
#define __COZINHA_H__

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "pedido.h"
#include "tarefas.h"

// Semaforos
sem_t s_bocas;
sem_t s_cozinheiros;
sem_t s_gorcons;
sem_t s_frigideiras;
sem_t s_balcao_garcons;
sem_t s_balcao_cozinheiros;

// Mutexes para indices
pthread_mutex_t mutex_coleta_garcon;
pthread_mutex_t mutex_entrega_cozinheiro;

// tamanho do balco para módulo
int balcao;

// Numero de cozinheiros
int num_cozinheiros;

// Numero de garcons
int num_garcons;

// Indices dos cozinheiros a colocar pratos
// e garcons a pegar
int index_cozinheiro, index_garcon;

// Vetor de pratos a serem entregues
prato_t** pratos_prontos;

// Threads garcons
pthread_t* garcons_threads;


extern void cozinha_init(int cozinheiros,
                         int bocas,
                         int frigideiras,
                         int garcons,
                         int tam_balcao);
extern void cozinha_destroy();
extern void processar_pedido(pedido_t p);

// Functions headers
void* fervendo_agua_thread(void* arg);
void* dourando_bacon_thread(void* arg);
void* esquentando_molho_thread(void* arg);
void prepara_carne(pedido_t pedido);
void prepara_sopa(pedido_t pedido);
void prepara_spaghetti(pedido_t pedido);
void* cozinheiro_thread(void* arg);
void* garcon_thread(void* arg);

#endif /*__COZINHA_H__*/
