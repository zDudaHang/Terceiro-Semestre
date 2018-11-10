#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int first_proc_id;
int ex_count = 1;

int fork_with_print() {
  int prior_id = getpid(); // Papai
  int id = fork();
  if (id == 0) {
    printf("fork,%d,%d\n", getpid(), prior_id);
    fflush(stdout);
  } else {
    printf("I am process %d, parent of %d!\n I have just forked it!\n", getpid(), id);
  }
  return id;
}

void finish_exercise() {
  while (wait(NULL) > 0);
  printf("start,%d,%d\n", ex_count, getpid());
  fflush(stdout);
  ex_count++;
}

void exercise_1() {
  for (int i = 0; i < 3; ++i) {
    if (!fork_with_print()) {
      exit(1);
    }
  }
  finish_exercise();
}

void exercise_2() {
  for (int i = 0; i < 3; i++) {
    if (!fork_with_print()) {
      if (i == 0) {
        for (int j = 0; j < 2; j++) {
          if (!fork_with_print()) {
            if (j == 0) {
              if (!fork_with_print()) {
                exit(1);
              }
            } else {
              exit(1);
            }
          }
        }
      } else {
        exit(1);
      }
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();

  if (fork_with_print()) {
    if(fork_with_print()) {
      if (fork_with_print()) {
        //termina o pai
      } else {
        //filho 3 nível 1
      }
    } else {
      //filho 2 nível 1
    }
  } else {
    //filho 1 nível 1
  }
}

void exercise_3() {
  for (int i = 0; i < 2; i++) {
    if(!fork_with_print()) {
      for(int j = 0; j < 2; j++) {
        if(!fork_with_print()) {
          exit(1);
        }
      }
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();
}

void exercise_4() {
  for (int i = 0; i < 3; i++) {
    if (!fork_with_print()) {
      if (i != 2) {
        for (int j = 0; j < 2; j++) {
          if (!fork_with_print()) {
            exit(1);
          }
        }
      }
      // Deveria criar um ramo else e dar exit(1) ??
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();
}

void exercise_5() {
  for (int i = 0; i < 2; i++) {
    if (!fork_with_print()) {
      if (i == 0) {
        for (int j = 0; j < 2; j++) {
          if (!fork_with_print()) {
            if (!fork_with_print()) {
              exit(1);
            }
          }
        }
      } else {
        if (!fork_with_print()) {
          for (int j = 0; j < 2; j++) {
            if (!fork_with_print()) {
              exit(1);
            }
          }
        }
      }
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();
}

void exercise_6() {
  for (int i = 0; i < 2; i++) {
    if (!fork_with_print()) {
      if (i == 0) { // Entra o primeiro filho
        for (int j = 0; j < 2; j++) {
          if (!fork_with_print()) {
            if (j == 0) {
              if (!fork_with_print()) {
                for (int k = 0; k < 2; k++) {
                  exit(1);
                }
              }
            }
          }
        }
      }
    }
    break;
  }
  while(wait(NULL) > 0);
  finish_exercise();
}

void exercise_7() {
  for (int i = 0; i < 3; i++) {
    if (!fork_with_print()) { // Aqui entra o filho de  grau 1
      if (!fork_with_print()) { // Aqui entra o filho de grau 2
        for (int j = 0; j < 2; j++) {
          if (!fork_with_print()) {
            exit(1);
          }
        }
      }
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();
}

void exercise_8() {
  if (!fork_with_print()) { // Crio um filho e ele entra aqui
    for (int i = 0; i < 3; i++) {
      if (!fork_with_print()) {
        if (i == 1) {
          if (!fork_with_print()) { // O segundo filho entra aqui
            for (int j = 0; j < 3; j++) {
              if (!fork_with_print()) { // Cria tres filhos
                exit(1);
              }
            }
          }
        }
      }
    }
  }
  while (wait(NULL) > 0);
  finish_exercise();
}

int main (int argc, char** argv) {
  printf("start,%d,%d\n",ex_count, getpid());
  ex_count++;
  first_proc_id = getpid();
  fflush(stdout);
  exercise_1();
  exercise_2();
  exercise_3();
  exercise_4();
  exercise_5();
  exercise_6();
  exercise_7();
  exercise_8();
  printf("end,0,8\n");
  return 0;
}
