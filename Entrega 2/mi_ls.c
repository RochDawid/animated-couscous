/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    if (argv[2][strlen(argv[2]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero\n");
        return -1;
    }

    char buffer[TAMBUFFER];
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0; 
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    memset(buffer,0,TAMBUFFER);
    int numEntradas = mi_dir(argv[2],buffer);
    if (numEntradas == -1) return -1;

    fprintf(stderr,"Total: %d\n",numEntradas);

    if (numEntradas > 0) {
        fprintf(stderr,"Tipo    Permisos    mTime                       Tamaño      Nombre        \n");
        fprintf(stderr,"--------------------------------------------------------------------------\n");

        for (int i=0;i<numEntradas;i++) {
            struct entrada entrada;
            struct inodo inodo;
            struct tm *ts;
            char mtime[80];
            char permisos[3];

            mi_read_f(p_inodo,&entrada,i*sizeof(struct entrada),sizeof(struct entrada));
            leer_inodo(entrada.ninodo,&inodo);

            ts = localtime(&inodo.mtime);
            strftime(mtime, sizeof(mtime), "%Y-%m-%d %H:%M:%S", ts);

            if ((inodo.permisos & 6) == 6) {
                strcpy(permisos,"rw-");
            } else if ((inodo.permisos & 4) == 4) {
                strcpy(permisos,"r--");
            } else if ((inodo.permisos & 2) == 2) {
                strcpy(permisos,"-w-");
            } else {
                strcpy(permisos,"---");
            }
            fprintf(stderr,"%c       %s         %s         %d          %s\n", inodo.tipo,permisos, mtime, inodo.tamEnBytesLog,entrada.nombre);
        }
    }

    return bumount();
}