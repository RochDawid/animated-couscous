/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_cp_f disco /origen/nombre /destino\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    if (argv[2][strlen(argv[2]) - 1] == '/') {
        fprintf(stderr,"Error: El destino debe ser un fichero\n");
        return -1;
    }

    unsigned int p_inodo_dir = 0, p_inodo_dir2 = 0;
    unsigned int p_inodo = 0, p_inodo2 = 0;
    unsigned int p_entrada = 0, p_entrada2 = 0;
    char reservar = 0;
    int error;
    struct entrada entrada;
    char camino_nuevo[strlen(argv[3])];
    int leidos = 0, offset = 0, tambuffer = BLOCKSIZE*4; //PROBAR tambuffer=1500
    char buffer[tambuffer];

    if ((error = buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    mi_read_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));

    memset(camino_nuevo,0,strlen(camino_nuevo));
    strcpy(camino_nuevo,argv[3]);
    strcat(camino_nuevo,entrada.nombre);

    mi_creat(camino_nuevo,6);

    memset(buffer,0,tambuffer);
    leidos = mi_read(argv[2],buffer,offset,tambuffer);
    if (leidos == -1) return -1;

    while (leidos > 0) {
        mi_write(camino_nuevo,buffer,offset,tambuffer);
        offset += tambuffer;
        memset(buffer,0,tambuffer);   
        leidos = mi_read(argv[2], buffer, offset, tambuffer);     
    }

    return bumount();
}
