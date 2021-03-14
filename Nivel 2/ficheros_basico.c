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

    printf("\nDATOS DEL SUPERBLOQUE INITSB\n");
        printf("posPrimerBloqueMB = %u\n",SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %u\n",SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %u\n",SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %u\n",SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %u\n",SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %u\n",SB.posUltimoBloqueDatos);
        printf("posInodoRaiz = %u\n",SB.posInodoRaiz);
        printf("posPrimerInodoLibre = %u\n",SB.posPrimerInodoLibre);
        printf("cantBloquesLibres = %u\n",SB.cantBloquesLibres);
        printf("cantInodosLibres = %u\n",SB.cantInodosLibres);
        printf("totBloques = %u\n",SB.totBloques);
        printf("totInodos = %u\n",SB.totInodos);
    return bwrite(posSB,&SB);
}

int initMB() {
    struct superbloque SB;
    if (bread(posSB,&SB) == -1) {
        perror("Error leyendo dispositivo virtual");
        return -1;
    }

    printf("\nDATOS DEL SUPERBLOQUE INITMB ANTES\n");
        printf("posPrimerBloqueMB = %u\n",SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %u\n",SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %u\n",SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %u\n",SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %u\n",SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %u\n",SB.posUltimoBloqueDatos);
        printf("posInodoRaiz = %u\n",SB.posInodoRaiz);
        printf("posPrimerInodoLibre = %u\n",SB.posPrimerInodoLibre);
        printf("cantBloquesLibres = %u\n",SB.cantBloquesLibres);
        printf("cantInodosLibres = %u\n",SB.cantInodosLibres);
        printf("totBloques = %u\n",SB.totBloques);
        printf("totInodos = %u\n",SB.totInodos);

    unsigned char buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    for (int i = SB.posPrimerBloqueMB;i < SB.posUltimoBloqueMB;i++) {
        if (bwrite(i,buf) == -1) {
            perror("Error escribiendo en dispositivo virtual");
            return -1;
        }
    }

    printf("\nDATOS DEL SUPERBLOQUE INITMB DESPUÉS\n");
        printf("posPrimerBloqueMB = %d\n",SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",SB.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",SB.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",SB.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",SB.cantInodosLibres);
        printf("totBloques = %d\n",SB.totBloques);
        printf("totInodos = %d\n",SB.totInodos);

    return 0;
}

int initAI() {
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    struct superbloque SB;
    if (bread(posSB,&SB) == -1) {
        perror("Error leyendo dispositivo virtual");
        return -1;
    }
    int contInodos = SB.posPrimerInodoLibre + 1;

    printf("\nDATOS DEL SUPERBLOQUE INITAI ANTES\n");
        printf("posPrimerBloqueMB = %d\n",SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",SB.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",SB.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",SB.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",SB.cantInodosLibres);
        printf("totBloques = %d\n",SB.totBloques);
        printf("totInodos = %d\n",SB.totInodos);

    
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++) {
            inodos[j].tipo = 'l';
            if (contInodos < SB.totInodos) {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else {
                inodos[j].punterosDirectos[0] = UINT_MAX;
            }
        }
        if (bwrite(i, inodos) == -1) {
            perror("Error escribiendo en dispositivo virtual");
            return -1;
        }
    }

    printf("\nDATOS DEL SUPERBLOQUE INITAI DESPUÉS\n");
        printf("posPrimerBloqueMB = %d\n",SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",SB.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",SB.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",SB.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",SB.cantInodosLibres);
        printf("totBloques = %d\n",SB.totBloques);
        printf("totInodos = %d\n",SB.totInodos);

    return 0;
}
