/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Error. Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    int permisos = atoi(argv[2]);
    if (permisos >= 0 && permisos <= 7) {
        unsigned int p_inodo_dir = 0;
        unsigned int p_inodo = 0;
        unsigned int p_entrada = 0;
        char reservar = 1;

        buscar_entrada(argv[3],&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos);
        return bumount();
    }
    fprintf(stderr,"Error. Permisos inválidos\n");
    return -1;
}