#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//      principal (ao finalizar): "Processo principal %d finalizado\n"
// filhos e netos (ao finalizar): "Processo %d finalizado\n"
//    filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar

// For(int i = 0; i < 3; i++) {
	// if (fork()==0) {
		// break;
	// }
// }

int main(int argc, char** argv) {
	int pid1 = fork(); //! Primeiro filho (f1)
	if (pid1 > 0) { //! Pai (p)
		int pid2 = fork(); //! Segundo filho (f2)
		if (pid2 > 0) { //! p
			printf("Processo principal %d finalizado \n", getpid());
		} else {
			if (pid2 == 0) { //! f2
				int pid3 = fork();
				if (pid3 > 0) { //! f2
					int pid4 = fork();
					if (pid4 > 0) { //! f2
						int pid5 = fork();
						if (pid5 > 0) { //! f2
							printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f1
							printf("Processo %d finalizado\n", getpid());
						} else {
							if(pid5 == 0) {
								printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f3 de f1
								sleep(5);
								printf("Processo %d finalizado\n", getpid());
							}
						}
					} else {
						if(pid4 == 0) { //! f2_f2
							printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f3 de f1
							sleep(5);
							printf("Processo %d finalizado\n", getpid());
						}
					}
				} else {
					if (pid3 == 0) { //! f1_f2
						printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f3 de f1
						sleep(5);
						printf("Processo %d finalizado\n", getpid());
					}
				}
			}
		}
	} else {
		if (pid1 == 0) { //! f1
			int pid3 = fork();
			if (pid3 > 0) { //! f1
				int pid4 = fork(); 
				if (pid4 > 0) { //! f1
					int pid5 = fork();
					if (pid5 > 0) { //! f1
						printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f1
						printf("Processo %d finalizado\n", getpid());
					} else {
						if (pid5 == 0) { //!f3_f1
							printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f3 de f1
							sleep(5);
							printf("Processo %d finalizado\n", getpid());
						}
					}
				} else {
					if (pid4 == 0) { //! f2_f1
							printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f2 de f1
							sleep(5);
							printf("Processo %d finalizado\n", getpid());
					}
				}
			} else {
				if (pid3 == 0) { //! f1_f1
					printf("Processo %d, filho de %d\n", getpid(), getppid()); // Printa o f1 de f1
					sleep(5);
					printf("Processo %d finalizado\n", getpid());
				}
			}
		} else {
	
		}
	}
    //printf("Processo principal %d finalizado\n", getpid());   
    while (wait(NULL) > 0); // Fazer todos os pais esperarem os filhos 
    return 0;
}
