#include "dagostrophism.h" 

//Habilita a função fdopen(). 200809L corresponde ao POSIX de 2008, mas um valor de 1 (POSIX.1, 1990) já seria suficiente.
#define _POSIX_C_SOURCE 200809L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#define N_CHILDREN 4

//                (pai)
//                  |
//    +--------+----+----+--------+
//    |        |         |        |
// filho_1  filho_2   filho_3  filho_4

// ~~~ printfs  ~~~
//  pai (ao terminar): "Palavras dagostróficas: %d\n"

// IMPORTANTE: consulte ../exercicio4-single_proc para a implementação 
// com um único processo. Sua implementação deve ter o mesmo comportamento!

// Obs:
// - pai deve usar pipes para se comunicar com os filhos (cf. exemplo-pipes)
// - pai deve ler palavras e distribuir elas entre o filhos
//   +---------------------------------------------------------
//   |      entrada | plv1 plv2 plv3 plv4 plv5 plv6 plv8  ...
//   |--------------+------------------------------------------
//   |filho destino | 1    2    3    4    1    2    3     ...
//   +---------------------------------------------------------
// - cada filho deve enviar ao pai o número de palavras dagostróficas
// - o pai soma as contagens e apenas ele deve mostrar a mensagem com o total


int child_main(int downstream, int upstream);

int main(int argc, char** argv) {
    //Total de palavras dagostróficas
    int total = 0;
   
    //Matriz de pipes              //               (read_end) | (write_end)
    //       2x2                   //                    0     |       1
    int downstream[N_CHILDREN][2]; // (filho_1) 0   up[0][0]   |  up[0][1]
    int   upstream[N_CHILDREN][2]; // (filho_2) 1   up[1][0]   |  up[1][1]  
                                   // (filho_3) 2   up[2][0]   |  up[2][1]  
                                   // (filho_4) 3   up[3][0]   |  up[3][1]  

    // ....
    int pidsFilhos[4];
    for (int i = 0; i < N_CHILDREN; i++) {
        /* Foi colocado antes do fork para os filhos não sobreescrevem os pipes do pai */
          if (pipe(downstream[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        if (pipe(upstream[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        fflush(stdout);
        pidsFilhos[i] = fork();

         /* Processo Filho ! */
        if (pidsFilhos[i] == 0) {
            close(downstream[i][1]); /* Fechando a escrita do filho */
            close(upstream[i][0]); /* Fechando a leitura do filho */
        /* No metodo child_main: O filho ja escreve para o pai. Param: O down e o up que ele usa */
            return child_main(downstream[i][0],upstream[i][1]); 
        /* downstream: Pai manda e filho le. upstream: Filho manda e pai le */
        }
    }
    /* Processo pai ! */
    //if (pidsFilhos[3] > 0) {
        // Botar o close nos filhos
        printf("Pai entrou aqui\n");
        for (int i = 0; i < N_CHILDREN; i++) {
            close(downstream[i][0]); /* Fechando a leitura do pai */
            close(upstream[i][1]); /* Fechando a escrita do pai */
        }

        /* Aqui o papai deve escrever para os filhos */
        int i = 0;
        char entradaParaFilho[4096] = {0};
        /* Aqui o papai pega as palavras do .txt e distribui para os filhos*/
        while (scanf("%s", entradaParaFilho) != EOF) {
            printf("Entrou: %s\n", entradaParaFilho);
            write(downstream[i%4][1], entradaParaFilho, strlen(entradaParaFilho)); /* Para guardar um lugar para o termino da String */
            write(downstream[i%4][1], " ", 1); // Entrada1: ana Entrada2: bob e nao tiver o " ", ele vai ler: anabob
            i++;
        }
        for (int i = 0; i < N_CHILDREN; i++) {
            close(downstream[i][1]); /* Para o filho saber que o pai nao mandara mais nada */
        }
        /* Aqui o pai espera todos os filhos */
        int status;

        /* Leitura das contagens dos filhos */
        for (int i = 0; i < N_CHILDREN; i++) {
            waitpid(pidsFilhos[i], &status, 0);
            if (WIFEXITED(status) > 0) {
                if(WEXITSTATUS(status) == 0) { // Deu tudo ok
                    char buf[16] = {0};
                    int bytes = read(upstream[i][0], buf, 16);
                    buf[bytes] = ' ';
                    printf("O filho entregou = %s\n", buf);
                    total += atoi(buf);
                } else {
                    printf("Filho morreu com o código: %d\n", status);
                }
            }
        }
    //}
   
    printf("Palavras dagostróficas: %d\n", total);
    return 0;
}

int child_main(int downstream, int upstream) {
    int total = 0;
    // scanf() só funcionado com FILE*. 
    // cria um FILE* para acesso ao pipe downstream
    FILE* downstream_file = fdopen(downstream, "r");

    char word[4096];
    while (fscanf(downstream_file, "%4096s", word) > 0)
        total += is_dagostrophic(word);

    //Envia o total como uma string. usa word como buffer
    sprintf(word, "%d", total);
    write(upstream, word, strlen(word)+1);
    fclose(downstream_file); // tambem fecha downstream
    close(upstream);         // fecha nosso lado do pipe
    return 0;
}
