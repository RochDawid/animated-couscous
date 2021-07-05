/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_cp disco /origen/nombre /destino/\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    if (argv[2][strlen(argv[2]) - 1] != '/') {
        fprintf(stderr,"Error: El destino debe ser un directorio\n");
        return -1;
    }

    //mi_unlink(argv[2]);

    unsigned int p_inodo_dir = 0, p_inodo_dir2 = 0;
    unsigned int p_inodo = 0, p_inodo2 = 0;
    unsigned int p_entrada = 0, p_entrada2 = 0;
    char reservar = 0;
    int error;
    struct entrada entrada;
    char camino_nuevo[strlen(argv[3])];

    buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6);
    mi_read_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));

    memset(camino_nuevo,0,strlen(camino_nuevo));
    strcpy(camino_nuevo,argv[3]);
    strcat(camino_nuevo,entrada.nombre);
    strcat(camino_nuevo,"/");

    //mi_link(argv[2],camino_nuevo);
    reservar = 1;
    if ((error = buscar_entrada(camino_nuevo,&p_inodo_dir2,&p_inodo2,&p_entrada2,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return 0;
    }

    struct inodo inodo, inodo2;
    leer_inodo(p_inodo,&inodo);
    leer_inodo(p_inodo2,&inodo2);

    inodo2.nlinks = inodo.nlinks;
    inodo2.numBloquesOcupados = inodo.numBloquesOcupados;
    for (int i = 0;i < inodo.numBloquesOcupados;i++) {
        reservar_bloque();
    }
    inodo2.permisos = inodo.permisos;
    inodo2.tamEnBytesLog = inodo.tamEnBytesLog;
    inodo2.tipo = inodo.tipo;
    for (int i = 0;i < 12;i++) {
        inodo2.punterosDirectos[i] = inodo.punterosDirectos[i];
        
    }

    for (int i = 0;i < 3;i++) {
        inodo2.punterosIndirectos[i] = inodo.punterosIndirectos[i];
    }

    escribir_inodo(p_inodo2,inodo2);

    return bumount();
}
