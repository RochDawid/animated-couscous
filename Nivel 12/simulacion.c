#include "directorios.h"
#include "simulacion.h"

static int acabados = 0;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
     acabados++;
  }
}

int main(int argv, char **argv) {
    if (argv != 2) {
        fprintf(stderr,"Sintaxis: ./simulacion <disco>\n");
        return -1;
    }
}
