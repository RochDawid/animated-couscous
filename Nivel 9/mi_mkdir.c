/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    if (argv[3][strlen(argv[3]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero");
        return -1;
    }
    int permisos = atoi(argv[2]);
    if (permisos >= 0 && permisos <= 7) {
        mi_creat(argv[3],permisos);
        return bumount();
    }
    fprintf(stderr,"Error: modo inválido: <<%d>>\n", permisos);
    return -1;
}