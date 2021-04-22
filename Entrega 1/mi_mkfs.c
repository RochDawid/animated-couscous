/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros_basico.h"

int main(int argc, char **argv) {
    bmount(argv[1]);
    unsigned char buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    unsigned int nbloques = atoi(argv[2]);
    unsigned int ninodos = nbloques/4;
        
    for (int i = 0;i < nbloques;i++) {
        bwrite(i,buf);
    }
    initSB(nbloques,ninodos);
    initMB();
    initAI();

    reservar_inodo('d', 7);

    return bumount();
}