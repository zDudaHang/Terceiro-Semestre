#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// |         usado como 2o retorno! <-----+
// v                                      v
double* load_vector(const char* filename, int* out_size);

typedef struct {
    int begin;
    int end;
    double* a;
    double* b;
    double sum;
} args_struct;


void* calcVector(void* args) {
    double sum = 0;
    args_struct* actual_args = args;
    int begin = actual_args->begin;
    int end = actual_args->end;
    double* a = actual_args->a;
    double* b = actual_args->b;
    for (int i = begin; i < end; i++) {
        sum += a[i] * b[i];
    }
    actual_args->sum = sum;
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n", 
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }

    //Calcula produto escalar. Paralelize essa parte
    double result = 0;
    pthread_t threads[n_threads];
    int divInteger = a_size / n_threads;
    args_struct *thread_args[n_threads];
    for (int i = 0; i < n_threads; i++) {
        thread_args[i] = malloc(sizeof(args_struct));
        thread_args[i]->a = a;
        thread_args[i]->b = b;
        thread_args[i]->begin = divInteger*i;
        thread_args[i]->end = thread_args[i]->begin + divInteger;
    }
    
    thread_args[n_threads-1]->end = a_size;

    for (int i = 0; i < n_threads; i++) {
        pthread_create(&threads[i], NULL, calcVector, thread_args[i]);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
        result += thread_args[i]->sum;
        printf("Thread %ld encerrou.\n", threads[i]);
    }

    //Imprime resultado
    printf("Produto escalar: %g\n", result);    

    //Libera memória
    free(a);
    free(b);
    for (int i = 0; i < n_threads; i++) {
        free(thread_args[i]);
    }
    return 0;
}
