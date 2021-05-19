/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    // comprobamos la sintaxis
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_mkdir <disco> <permisos> </ruta_directorio>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    // comprobamos que no se trate de un fichero
    if (argv[3][strlen(argv[3]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero\n");
        return -1;
    }
    int permisos = atoi(argv[2]);
    if (permisos >= 0 && permisos <= 7) { // si los permisos introducidos son válidos
        if (!(strlen(argv[3]) == 1 && argv[3][0] == '/')) {
            mi_creat(argv[3],permisos); // creamos el directorio
        }
        
        return bumount();
    }
    fprintf(stderr,"Error: modo inválido: <<%d>>\n", permisos);
    return -1;
}