#include "ficheros_basico.h"

int main(int argc, char **argv) {
    if (bmount(argv[1]) != -1) {
        unsigned char buf[BLOCKSIZE];
        memset(buf,0,BLOCKSIZE);
        unsigned int nbloques = atoi(argv[2]);
        unsigned int ninodos = nbloques/4;
        initSB(nbloques,ninodos);
        initMB();
        initAI();
        for (int i = 0;i < nbloques;i++) {
            if (bwrite(i,buf) == -1) {
                perror("Error escribiendo");
                return -1;
            }
        }

        return bumount();
    }

    perror("Error montando el dispositivo virtual");
    return -1;
}