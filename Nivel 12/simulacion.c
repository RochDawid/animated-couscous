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
    signal(SIGCHLD,reaper);
    if (argv != 2) {
        fprintf(stderr,"Sintaxis: ./simulacion <disco>\n");
        return -1;
    }

    if (bmount(argv[1]) < 0) return -1;

    mi_creat("/simul_aaaammddhhmmss/",7);
    for (int numProcesos = 1; numProcesos <= NUMPROCESOS;numProcesos++) {
        pid_t pid = fork();
        if (pid = 0) {
            if (bmount(argv[1]) < 0) {
                char *ruta = strcat("/simul_aaaammddhhmmss/proceso_PID",);
                mi_creat(ruta,7);
                strcat(ruta,7);
                mi_creat(ruta,7);
                int random = srand(time(NULL) + getpid());
                for (int nEscritura = 1;nEscritura <= NUMESCRITRAS;nEscritura++) {
                    struct REGISTRO registro;
                    registro.fecha = time(NULL);
                    registro.pid = getpid();
                    registro.nEscritura = nEscritura;
                    registro.nRegistro = 
                }
            }
        }
    }

    return bumount();
}
