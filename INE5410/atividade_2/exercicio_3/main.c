#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

//    (pai)
//      |
//   filho_1 

// ~~~ printfs  ~~~
//        filho (ao iniciar): "Processo %d iniciado\n"
//          pai (ao iniciar): "Processo pai iniciado\n"
// pai (após filho terminar): "Filho retornou com código %d,%s encontrou silver\n"
//                            , onde %s é
//                              - ""    , se filho saiu com código 0
//                              - " não" , caso contrário

// Obs:
// - processo pai deve esperar pelo filho
// - filho deve trocar seu binário para executar "grep silver text"
//   + dica: use execl(char*, char*...)
//   + dica: em "grep silver text",  argv = {"grep", "silver", "text"}

int main(int argc, char** argv) {
    printf("Processo principal iniciado\n");
    int pid = fork();
    if (pid > 0) { //! Processo pai
    	printf("Processo pai iniciado\n");
    	int status;
    	do {
    		waitpid(pid, &status, 0); // Esperando o filho mudar de estado
    		if (WIFEXITED(status) > 0) { // Se o filho saiu certinho, ele retorna 1(Bunitinho). Senão, 0.
    			break;
    		}
    	} while (1);

    	status = WEXITSTATUS(status); // Aqui ele pega o retorno da chamda do grep do filho
    	if(status == 0) {
    		printf("Filho retornou com código 0, encontrou silver\n");
    	} else {
    		printf("Filho retornou com código %d, não encontrou silver\n", status);
    	}
    } else {
    	if (pid == 0) { //! Processo filho
    		printf("Processo %d iniciado \n", getpid());
    		fflush(stdout);
    		execlp("grep", "grep", "silver", "text", NULL);
    	}
    }
    // ....
    
    return 0;
}
