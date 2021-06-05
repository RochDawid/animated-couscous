/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

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

int main(int argc,char **argv) {
    signal(SIGCHLD,reaper);
    if (argc != 2) {
        fprintf(stderr,"Sintaxis: ./simulacion <disco>\n");
        return -1;
    }

    if (bmount(argv[1]) < 0) return -1;

    char ruta[100];
    struct tm *ts;
    time_t timer = time(NULL);

    ts = localtime(&timer);
    strftime(ruta, sizeof(ruta), "/simul_%Y%m%d%H%M%S/", ts);

    // creamos el directorio de simulación
    if (mi_creat(ruta,7) == -1) return -1;
    for (int numProcesos = 1; numProcesos <= NUMPROCESOS;numProcesos++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (bmount(argv[1]) < 0) {
                return -1;
            }
            char idProceso[5];
            sprintf(idProceso, "%d", getpid());
            // creación del directorio del proceso
            strcat(idProceso,"/");
            strcat(ruta,"proceso_");
            strcat(ruta,idProceso);
            if (mi_creat(ruta,7) == -1) return -1;
            // creación del fichero 'prueba.dat'
            strcat(ruta,"prueba.dat");
            if (mi_creat(ruta,7) == -1) return -1;
            srand(time(NULL) + getpid());
            int nEscritura = 1;
            // escritura de múltiples REGISTRO en el fichero creado
            for (;nEscritura <= NUMESCRITURAS;nEscritura++) {
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = nEscritura;
                registro.nRegistro = rand() % REGMAX;

                if (mi_write(ruta,&registro,registro.nRegistro*sizeof(struct REGISTRO),sizeof(struct REGISTRO)) == -1) {
                    fprintf(stderr,"fallo\n");
                    return -1;
                }
                //fprintf(stderr,"simulacion.c -> Escritura %d en %s\n",nEscritura,ruta);
                usleep(50000);
            }
            nEscritura--;
            fprintf(stderr,"PROCESO %d: Completadas %d escrituras en %s\n",numProcesos,nEscritura,ruta);
            if (bumount() < 0) return -1;
            exit(0);
        }
        usleep(0.2*1000000);
    }

    while (acabados < NUMPROCESOS) pause();

    if (bumount() < 0) return -1;
    exit(0);
}
