/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_rn disco /ruta/antiguo nuevo\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    /* if (argv[2][strlen(argv[2]) - 1] == '/' && argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr,"Antiguo y nuevo no son del mismo tipo\n");
        return -1;
    } */

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0, p_inodo2 = 0;
    unsigned int p_entrada = 0, p_entrada2 = 0;
    char reservar = 0;
    int error;
    struct entrada entrada;
    char camino_nuevo[strlen(argv[3])+1];

    if ((error = buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }
    mi_read_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));

    memset(camino_nuevo,0,strlen(camino_nuevo));
    camino_nuevo[0] = '/';
    strcat(camino_nuevo,argv[3]);

    if ((error = buscar_entrada(camino_nuevo,&p_inodo_dir,&p_inodo2,&p_entrada2,reservar,6)) != ERROR_NO_EXISTE_ENTRADA_CONSULTA) {
        fprintf(stderr, "No se pudo renombrar: El nombre ya existe en el directorio especificado.\n");
        return -1;
    }

    memset(entrada.nombre,0,TAMNOMBRE);
    strcpy(entrada.nombre,argv[3]);
    mi_write_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));

    return bumount();
}
