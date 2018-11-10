#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// pai (ao criar filho): "Contador: %d\n"
// pai (ao criar filho): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global n_threads*1000
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;

void* increment(void* p) {
	int n = *((int *)p);
	for (int i = 0; i < n; i++) {
		contador_global++;
	}
	pthread_exit(0);
}

int main(int argc, char* argv[]) {
    int n_threads = 0, loops = 0;
    if (argc < 2) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads\n", argv[0]);
        return 1;
    } else {
    	n_threads = atoi(argv[1]);
    	pthread_t threads[n_threads];
    	loops = 1000 * n_threads;
    	for (int i = 0; i < n_threads; i++) {
    		pthread_create(&threads[i], NULL, increment, &loops);
    	}
    	for (int i = 0; i < n_threads; i++) {
    		pthread_join(threads[i], NULL);
    		printf("Thread %ld encerrou. Retorno = %d\n", threads[i], contador_global);
    	}
    }

    //...
    
    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", loops*n_threads);
    return 0;
}
