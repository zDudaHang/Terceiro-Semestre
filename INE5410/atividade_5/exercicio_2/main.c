#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

int produzir(int value);    //< definida em helper.c
void consumir(int produto); //< definida em helper.c
void *ProdutorFunc(void *arg);
void *ConsumidorFunc(void *arg);

int indice_produtor, indice_consumidor, tamanho_buffer;
int* buffer;
sem_t empty_values, fully_values;
pthread_mutex_t mutex_produtoras, mutex_consumidoras;

//Você deve fazer as alterações necessárias nesta função e na função
//ConsumidorFunc para que usem semáforos para coordenar a produção
//e consumo de elementos do buffer.
void *ProdutorFunc(void *arg) {
    //arg contem o número de itens a serem produzidos
    int max = *((int*)arg);
    for (int i = 0; i <= max; ++i) {
        int produto;
        if (i == max)
            produto = -1;          //envia produto sinlizando FIM
        else 
            produto = produzir(i); //produz um elemento normal
        sem_wait(&empty_values);
        pthread_mutex_lock(&mutex_produtoras);
        indice_produtor = (indice_produtor + 1) % tamanho_buffer; //calcula posição próximo elemento
        buffer[indice_produtor] = produto; //adiciona o elemento produzido à lista
        pthread_mutex_unlock(&mutex_produtoras);
        sem_post(&fully_values);
    }
    pthread_exit(0);
}

void *ConsumidorFunc(void *arg) {
    while (1) {
        sem_wait(&fully_values);
        pthread_mutex_lock(&mutex_consumidoras);
        indice_consumidor = (indice_consumidor + 1) % tamanho_buffer; //Calcula o próximo item a consumir
        int produto = buffer[indice_consumidor]; //obtém o item da lista
        pthread_mutex_unlock(&mutex_consumidoras);
        sem_post(&empty_values);

        //Podemos receber um produto normal ou um produto especial
        if (produto >= 0)
            consumir(produto); //Consome o item obtido.
        else
            break; //produto < 0 é um sinal de que o consumidor deve parar
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Uso: %s tamanho_buffer itens_produzidos n_produtores n_consumidores \n", argv[0]);
        return 0;
    }

    tamanho_buffer = atoi(argv[1]);
    int itens = atoi(argv[2]);
    int n_produtores = atoi(argv[3]);
    int n_consumidores = atoi(argv[4]);

    //Iniciando buffer
    indice_produtor = 0;
    indice_consumidor = 0;
    buffer = malloc(sizeof(int) * tamanho_buffer);
    int producer_vs_consumer = n_consumidores - n_produtores;

    //Arrays das Threads:
    pthread_t consumidoras[n_consumidores];
    pthread_t produtoras[n_produtores];

    //Semaforos
    sem_init(&empty_values, 0, tamanho_buffer);
    sem_init(&fully_values, 0, 0);
    pthread_mutex_init(&mutex_produtoras, NULL);
    pthread_mutex_init(&mutex_consumidoras, NULL);

    //Criando as threads
    for (int i = 0; i < n_produtores; i++) {
        pthread_create(&produtoras[i], NULL, ProdutorFunc, (void*)&itens);
    }
    for (int i = 0; i < n_consumidores; i++) {
        pthread_create(&consumidoras[i], NULL, ConsumidorFunc, NULL);
    }

    //Esperando as threads
    for (int i = 0; i < n_produtores; i++) {
        pthread_join(produtoras[i], NULL);
    }
    if (producer_vs_consumer > 0) {
        for (int i = 0; i < producer_vs_consumer; i++) {
            sem_wait(&empty_values);
            indice_produtor = (indice_produtor+1)%tamanho_buffer;
            buffer[indice_produtor] = -1;
            sem_post(&fully_values);
        }
    }
    for (int i = 0; i < n_consumidores; i++) {
        pthread_join(consumidoras[i], NULL);
    }
    //Destruindo os smeaforos
    sem_destroy(&fully_values);
    sem_destroy(&empty_values);
    pthread_mutex_destroy(&mutex_produtoras);
    pthread_mutex_destroy(&mutex_consumidoras);

    //Libera memória do buffer
    free(buffer);

    return 0;
}

