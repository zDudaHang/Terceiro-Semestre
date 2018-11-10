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

//Você deve fazer as alterações necessárias nesta função e na função
//ConsumidorFunc para que usem semáforos para coordenar a produção
//e consumo de elementos do buffer.
// Anotações:
// -> Semaforo deve começar com o tamanho maximo que ela pode inserir
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
        indice_produtor = (indice_produtor + 1) % tamanho_buffer; //calcula posição próximo elemento
        buffer[indice_produtor] = produto; //adiciona o elemento produzido à lista
        sem_post(&fully_values);
    }
    return NULL;
}

// Anotacoes:
// -> Esse semaforo deve comecar travando e so podera consumir quando tiver pelo menos um valor nele
// -> O consumidor deve travar no inicio
void *ConsumidorFunc(void *arg) {
    while (1) {
        //! Semaforo do Consumidor
        //! Wait -- e trava no 0
        //! Post ++
        sem_wait(&fully_values);
        indice_consumidor = (indice_consumidor + 1) % tamanho_buffer; //Calcula o próximo item a consumir
        int produto = buffer[indice_consumidor]; //obtém o item da lista
        sem_post(&empty_values);
        //Podemos receber um produto normal ou um produto especial
        if (produto >= 0)
            consumir(produto); //Consome o item obtido.
        else
            break; //produto < 0 é um sinal de que o consumidor deve parar
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s tamanho_buffer itens_produzidos\n", argv[0]);
        return 0;
    }

    tamanho_buffer = atoi(argv[1]);
    int n_itens = atoi(argv[2]);

    //Iniciando buffer
    indice_produtor = 0;
    indice_consumidor = 0;
    buffer = malloc(sizeof(int) * tamanho_buffer);
    pthread_t producer;
    pthread_t consumer;
    // AREA DA PRODUTORA
    // Este semaforo sera da produtora que decrementara ele
    sem_init(&empty_values, 0, tamanho_buffer);
    sem_init(&fully_values, 0, 0);

    pthread_create(&producer, NULL, ProdutorFunc, (void*) &n_itens);    

    // AREA DA CONSUMIDORA
    // Este semaforo sera da consumidora que decrementara ele

    pthread_create(&consumer, NULL, ConsumidorFunc, NULL);
   
    pthread_join(producer, NULL); 
    pthread_join(consumer, NULL);

    sem_destroy(&fully_values);
    sem_destroy(&empty_values);

    //Libera memória do buffer
    free(buffer);
    return 0;
}

