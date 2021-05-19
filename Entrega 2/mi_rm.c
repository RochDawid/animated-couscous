/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    // comprobamos la sintaxis
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_rm disco /ruta\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    // comprobamos que no se trate de un directorio
    if (argv[2][strlen(argv[2]) - 1] == '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un directorio\n");
        return -1;
    }
    
    mi_unlink(argv[2]); // deshacemos el enlace

    return bumount();
}