/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_rm_r disco /ruta_directorio\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    if (argv[2][strlen(argv[2]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero\n");
        return -1;
    }

    // no borramos el directorio raíz
    if (!(strlen(argv[2]) == 1 && argv[2][0] == '/')) {
        unsigned int p_inodo_dir = 0;
        unsigned int p_inodo = 0;
        unsigned int p_entrada = 0;
        char reservar = 0;
        int error;
        struct inodo inodo;
        if ((error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
            mostrar_error_buscar_entrada(error);
            return -1;
        }

        leer_inodo(p_inodo, &inodo);

        if (inodo.tipo == 'd' && inodo.tamEnBytesLog == 0) {
            fprintf(stderr,"Error: El directorio %s está vacío\n",argv[2]);
            return -1;
        } 
        mi_unlink_r(argv[2], p_inodo);
    }

    return bumount();
}