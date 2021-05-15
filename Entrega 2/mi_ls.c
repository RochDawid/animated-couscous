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
    char permisos[3]; 
    char reservar = 0;
    struct inodo inodo;
    int error;
    char mtime[80];
    struct tm *ts;

    if ((error = buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    leer_inodo(p_inodo, &inodo); // leemos el inodo de la entrada
    
    if ((inodo.permisos & 6) == 6){
        strcpy(permisos,"rw-");
    } else if ((inodo.permisos & 4) == 4) {
        strcpy(permisos,"r--");
    } else if ((inodo.permisos & 2) == 2) {
        strcpy(permisos,"w--");
    } else {
        strcpy(permisos, "---");
    }


    memset(buffer,0,TAMBUFFER);
    int numEntradas = mi_dir(argv[2],buffer);
    if (numEntradas == -1) return -1;

    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);

    fprintf(stderr,"Total: %d\n",numEntradas);
    fprintf(stderr,"Tipo    Permisos    mTime                       Tamaño  Nombre\n");
    fprintf(stderr,"--------------------------------------------------------------\n");

    int j = 0;
    for (int i=0;i<numEntradas;i++) {
        fprintf(stderr,"%c       %s         %s     %d      ", inodo.tipo,permisos, mtime, inodo.tamEnBytesLog);
        if (buffer[j] == '\t') j++;
        for (;j<strlen(buffer) && buffer[j] != '\t';j++) {
            fprintf(stderr,"%c",buffer[j]);
        }
        fprintf(stderr,"\n");
    }

    return bumount();
}