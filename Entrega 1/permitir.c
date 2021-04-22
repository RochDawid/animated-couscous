/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

int main(int argc,char **argv) {
    if (argc == 4) {
        bmount(argv[1]);
        mi_chmod_f(atoi(argv[2]),atoi(argv[3]));    
        return bumount();
    }
    fprintf(stderr,"Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
    return -1;
}
