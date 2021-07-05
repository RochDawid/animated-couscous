/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_mv disco /origen/nombre /destino/\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr,"Error: El destino debe ser un directorio\n");
        return -1;
    }

    if ((strlen(argv[2]) == 1 && argv[2][0] == '/')) {
        fprintf(stderr,"Error: El directorio raíz no puede ser movido a otro directorio\n");
        return -1;
    }

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;
    struct inodo inodo_dir;
    struct entrada entrada;
    char camino_nuevo[strlen(argv[3])];

    if ((error = buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }
    mi_read_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));

    memset(camino_nuevo,0,strlen(camino_nuevo));
    strcpy(camino_nuevo,argv[3]);
    strcat(camino_nuevo,entrada.nombre);

    if (!mi_link(argv[2],camino_nuevo)) {
        leer_inodo(p_inodo_dir, &inodo_dir);
        int nEntradas = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
        if (p_entrada != nEntradas-1) {
            struct entrada entrada;
            mi_read_f(p_inodo_dir,&entrada,inodo_dir.tamEnBytesLog-sizeof(struct entrada),sizeof(struct entrada));
            mi_write_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));
        }
        mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog-sizeof(struct entrada));
    }

    return bumount();
}
