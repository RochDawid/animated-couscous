#include "bloques.h"

int main(int argc, char **argv) {
    int nbloques = atoi(argv[2]);

    if (bmount(argv[1]) != -1) {
        unsigned char buf[BLOCKSIZE];
        memset(buf,0,BLOCKSIZE); // inicializar a 0 el buffer
        for (int i = 0;i < nbloques;i++) {
            if (bwrite(i,buf) == -1) { // si da error escribiendo
                return -1; // lo devolvemos
            }
        }

        return bumount();
    }
}