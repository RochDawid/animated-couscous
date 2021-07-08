/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_ls <disco> <flag> </ruta>\n");
        return -1;
    }

    int flag = 0;
    char *camino = argv[2];
    if (!strcmp(argv[2],"-s")) {
        camino = argv[3];
        flag = 1;
    } else if (argc != 3) {
        fprintf(stderr,"Flag no reconocido: flag -> ls simple: '-s'\n");
        return -1;
    }

    if (bmount(argv[1]) < 0) return -1;
    /* if (argv[2][strlen(argv[2]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero\n");
        return -1;
    } */

    char tipo;
    if (camino[strlen(camino) - 1] != '/'){
        tipo = 'f';
    } else {
        tipo = 'd';
    }

    char buffer[TAMBUFFER];
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0; 
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    memset(buffer,0,TAMBUFFER);
    int numEntradas = mi_dir(camino, buffer, tipo, flag);
    if (numEntradas == -1) return -1;

    fprintf(stderr,"%s\n",buffer);

    return bumount();
}