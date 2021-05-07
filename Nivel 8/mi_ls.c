/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Error. Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    int permisos = atoi(argv[2]);
    if (permisos >= 0 && permisos <= 7) {
        mi_chmod(argv[3],permisos);
        return bumount();
    }
    fprintf(stderr,"Error. Permisos inválidos\n");
    return -1;
}