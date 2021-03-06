#include "ficheros_basico.h"

int tamMB(unsigned int nbloques) {
    int tamMB = (nbloques/8)/BLOCKSIZE;
    if ((nbloques/8) % BLOCKSIZE != 0) { // en caso que necesitemos más bloques para los bytes restantes
        tamMB++;
    }
    return tamMB;
}

int tamAI(unsigned int ninodos) {
    int tamAI = (ninodos*INODOSIZE)/BLOCKSIZE;
    if ((ninodos*INODOSIZE) % BLOCKSIZE != 0) { // en caso que necesitemos más bloques para los bytes restantes
        tamAI++;
    }
    return tamAI;
}

int initSB(unsigned int nbloques, unsigned int ninodos) {
    struct superbloque SB;
    SB.posPrimerBloqueMB = tamSB + posSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    struct superbloque *pSB;
    *pSB = SB;
    bwrite(posSB,pSB);
}

int initMB() {
    unsigned char buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    struct superbloque SB;
    for (int i = SB.posPrimerBloqueMB;i <= SB.posUltimoBloqueMB;i++) {
        bwrite(i,buf);
    }
}

int initAI() {
    struct inodo inodo[BLOCKSIZE/INODOSIZE];
    int numInodo;
    struct inodo buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    struct superbloque SB;
    int primeraPos = SB.posPrimerBloqueAI;
}