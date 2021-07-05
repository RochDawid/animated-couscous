/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_rmdir disco /ruta_directorio\n");
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
        unsigned int p_inodo = 0, p_inodo2 = 0;
        unsigned int p_entrada = 0, p_entrada2 = 0;
        char reservar = 0;
        int error;
        struct inodo inodo;

        buscar_entrada(argv[2],&p_inodo_dir,&p_inodo,&p_entrada,reservar,6);
        leer_inodo(p_inodo,&inodo);

        //if (inodo.nlinks != 0) {
            int cant_entradas_inodo, num_entrada_inodo;
            struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
            char camino[strlen(argv[2]) + 50];
            leer_inodo(*p_inodo_dir, &inodo_dir);

            cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
            num_entrada_inodo = 0;
            int indice = 0;
            memset(entradas,0,BLOCKSIZE); // ponemos a 0 el buffer de lectura independientemente de la cantidad de entradas del inodo
            if (cant_entradas_inodo > 0) {
                int offset = 0;
                
                mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
                while (num_entrada_inodo < cant_entradas_inodo) {
                    for (;(num_entrada_inodo < cant_entradas_inodo) && indice < BLOCKSIZE/sizeof(struct entrada);num_entrada_inodo++,indice++) {
                        memset(camino,0,strlen(camino));
                        strcpy(camino,argv[2]);
                        strcat(camino,entradas[indice].nombre);
                        mi_unlink(camino);
                    }
                    if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                        indice = 0;
                        offset += BLOCKSIZE;

                        memset(entradas,0,BLOCKSIZE);
                        mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
                    } 
                }
            }

            mi_unlink(argv[2]);
        //}
    }
    return bumount();
}