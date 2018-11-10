#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

FILE* out;
sem_t control_a, control_b;

void *thread_a(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        sem_wait(&control_a);
        fprintf(out, "A");
        // Importante para que vocês vejam o progresso do programa
        // mesmo que o programa de vocês trave em um sem_wait().
        fflush(stdout);
        sem_post(&control_b);
    }
    return NULL;
}

void *thread_b(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        sem_wait(&control_b);
        fprintf(out, "B");
        fflush(stdout);
        sem_post(&control_a);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s iteracoes\n", argv[0]);
        return 1;
    }
    int iters = atoi(argv[1]);
    srand(time(NULL));
    out = fopen("result.txt", "w");

    pthread_t ta, tb;
    sem_init(&control_a, 0, 1);
    sem_init(&control_b, 0, 1);
    // Cria threads
    pthread_create(&ta, NULL, thread_a, &iters);
    pthread_create(&tb, NULL, thread_b, &iters);

    // Espera pelas threads
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    //Imprime quebra de linha e fecha arquivo
    fprintf(out, "\n");
    fclose(out);
    sem_destroy(&control_a);
    sem_destroy(&control_b);
  
    return 0;
}
