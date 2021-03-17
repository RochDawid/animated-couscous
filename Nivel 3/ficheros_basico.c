#include "ficheros_basico.h"

/*
    tamMB: calcula el tamaño en bloques necesario para el mapa de bits
    input: unsigned int nbloques
    output: int tamMB
    uses: -
    used by: initSB()
*/
int tamMB(unsigned int nbloques) {
    int tamMB = (nbloques/8)/BLOCKSIZE;
    if ((nbloques/8) % BLOCKSIZE != 0) { // en caso que necesitemos más bloques para los bytes restantes
        tamMB++;
    }
    return tamMB;
}

/*
    tamMB: calcula el tamaño en bloques del array de inodos
    input: unsigned int ninodos
    output: int tamAI
    uses: -
    used by: initSB()
*/
int tamAI(unsigned int ninodos) {
    int tamAI = (ninodos*INODOSIZE)/BLOCKSIZE;
    if ((ninodos*INODOSIZE) % BLOCKSIZE != 0) { // en caso que necesitemos más bloques para los bytes restantes
        tamAI++;
    }
    return tamAI;
}

/*
    initSB: inicializa los datos del superbloque
    input: unsigned int nbloques, unsigned int ninodos
    output: BLOCKSIZE on success / -1 on failure
    uses: tamMB(),tamAI(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
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

    return bwrite(posSB,&SB);
}

/*
    initMB: inicializa el mapa de bits
    input: none
    output: BLOCKSIZE on success / -1 on failure
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int initMB() {
    struct superbloque SB;
    bread(posSB,&SB);
    unsigned char buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    int bitsMetadatos = (tamSB+tamMB(SB.totBloques)+tamAI(SB.totInodos));
    int posbyte = (bitsMetadatos / 8);
    int posbit = (bitsMetadatos % 8);

    for (int i=0;i<=posbyte;i++) {
        buf[i]=255;
    }

    // int array con los valores de la potencia de 2
    int bits[] = {128,64,32,16,8,4,2,1};
    int resultat = 0;

    for (int i=0;i<posbit;i++) {
        resultat += bits[i];
    }

    buf[posbyte+1]=resultat;
    
    for (int i = SB.posPrimerBloqueMB;i <= SB.posUltimoBloqueMB;i++) {
        bwrite(i,buf);
    }
    // actualizar cantidad de bloques libres
    SB.cantBloquesLibres = SB.cantBloquesLibres - bitsMetadatos;

    return bwrite(posSB,&SB);
}

/*
    initAI: inicializa la lista de inodos libres
    input: none
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int initAI() {
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    struct superbloque SB;
    bread(posSB,&SB);
    int contInodos = SB.posPrimerInodoLibre + 1;
    
    // enlazamiento y escritura en memoria virtual de los inodos libres
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
        bwrite(i, inodos);
    }

    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit) {
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    bread(posSB,&SB);
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    unsigned char mascara = 128;
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit; // nos desplazamos al bit que hay que cambiar
    
    if (bit == 1) {
        bufferMB[posbyte] |= mascara; // poner bit a 1
    } else {
        bufferMB[posbyte] &= ~mascara; // poner bit a 0
    }

    bwrite(nbloqueabs,bufferMB); // cargar cambios del superbloque
}

int leer_bit (unsigned int nbloque) {

}

int reservar_bloque() {
    struct superbloque SB;
    bread(posSB,&SB);
    if (SB.cantBloquesLibres != 0) {
        unsigned char bufferaux[BLOCKSIZE];
        unsigned char bufferMB[BLOCKSIZE];
        memset(bufferaux,255,BLOCKSIZE);
        unsigned int posBloqueMB = SB.posPrimerBloqueMB;
        int encontrado = 0;
        for (;posBloqueMB<(SB.posUltimoBloqueMB) && encontrado==0;posBloqueMB++) {
            bread(posBloqueMB,bufferMB);
            if (memcmp(bufferMB,bufferaux,BLOCKSIZE) != 0) { // primer byte con un 0 en el MB
                encontrado = 1;
            }
        }
        unsigned int posbyte;
        encontrado = 0;
        for (int i=0;i<BLOCKSIZE && encontrado==0;i++) {
            if (bufferMB[i] < 255) {
                posbyte = i;
                encontrado = 1;
            }
        }

        unsigned char mascara = 128;
        unsigned int posbit = 0;
        while (bufferMB[posbyte] & mascara) { // encontrar el primer bit a 0
            bufferMB[posbyte] <<= 1; // desplazamiento de bits a la izquierda
            posbit++;
        }
        unsigned int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
        escribir_bit(nbloque, 1);
        SB.cantBloquesLibres--;
        bwrite(posSB,&SB);
        memset(bufferaux,0,BLOCKSIZE);
        bwrite(nbloque,bufferaux);
        
        return nbloque;
    } else {
        perror("No hay bloques libres en el dispositivo virtual");
        return -1;
    }
}

int liberar_bloque(unsigned int nbloque) {

}

int escribir_inodo(unsigned int ninodo, struct inodo inodo) {

}

int leer_inodo(unsigned int ninodo, struct inodo *inodo) {

}

int reservar_inodo(unsigned char tipo, unsigned char permisos) {

}
