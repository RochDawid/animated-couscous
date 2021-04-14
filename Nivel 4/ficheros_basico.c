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
    tamAI: calcula el tamaño en bloques del array de inodos
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
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB,255,BLOCKSIZE);
    int bitsMetadatos = (tamSB+tamMB(SB.totBloques)+tamAI(SB.totInodos));

    int nbloqueabs = bitsMetadatos/(BLOCKSIZE*8);
    int posbyte = (bitsMetadatos / 8);
    int posbit = (bitsMetadatos % 8);
    for (int i = SB.posPrimerBloqueMB;i < SB.posPrimerBloqueMB+nbloqueabs;i++) {
        bwrite(i,bufferMB);
    }

    // int array con los valores de la potencia de 2
    int bits[] = {128,64,32,16,8,4,2,1};
    int resultat = 0;

    for (int i=0;i<posbit;i++) {
        resultat += bits[i];
    }
    posbyte %= BLOCKSIZE; // localizar posición dentro del bloque
    bufferMB[posbyte]=resultat;

    for (int i=posbyte+1;i<BLOCKSIZE;i++) {
        bufferMB[i]=0;
    }

    bwrite(nbloqueabs+SB.posPrimerBloqueMB,bufferMB);
    memset(bufferMB,0,BLOCKSIZE);
    for (int i = nbloqueabs+SB.posPrimerBloqueMB+1;i <= SB.posUltimoBloqueMB;i++) {
        bwrite(i,bufferMB);
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

/*
    escribir_bit: esta función escribe el valor indicado por el parámetro bit: 0 (libre) ó 
		1 (ocupado) en un determinado bit del MB que representa el bloque nbloque.
    input: unsigned int nbloque, unsigned int bit
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int escribir_bit(unsigned int nbloque, unsigned int bit) {
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    bread(posSB,&SB);
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    bread(nbloqueabs, bufferMB);
    unsigned char mascara = 128;
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit; // nos desplazamos al bit que hay que cambiar
    
    if (bit == 1) {
        bufferMB[posbyte] |= mascara; // poner bit a 1
    } else {
        bufferMB[posbyte] &= ~mascara; // poner bit a 0
    }

    return bwrite(nbloqueabs,bufferMB); // cargar cambios del superbloque
}

/*
    leer_bit: lee un determinado bit del MB y devuelve el valor del bit leído.
    input: unsigned int nbloque, unsigned int bit
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
char leer_bit (unsigned int nbloque) {
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];

    bread(posSB,&SB);
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    bread(nbloqueabs,bufferMB);
    unsigned char mascara = 128;
    posbyte = posbyte % BLOCKSIZE;
    mascara >>= posbit; // nos desplazamos al bit que hay que cambiar
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    return mascara;
}

/*
    reservar_bloque: encuentra el primer bloque libre, consultando el MB, lo ocupa (con la ayuda de la función escribir_bit()) y devuelve su posición.
    input: none
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int reservar_bloque() {
    struct superbloque SB;
    bread(posSB,&SB);

    if (SB.cantBloquesLibres != 0) {
        unsigned char bufferaux[BLOCKSIZE];
        unsigned char bufferMB[BLOCKSIZE];
        memset(bufferaux,255,BLOCKSIZE);
        unsigned int posBloqueMB = SB.posPrimerBloqueMB;
        int encontrado = 0;
        for (;(posBloqueMB<=SB.posUltimoBloqueMB) && encontrado==0;posBloqueMB++) {
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
        posBloqueMB--;
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
    }
    
    perror("No hay bloques libres en el dispositivo virtual");
    return -1;
}

/*
    liberar_bloque:libera un bloque determinado
    input: unsigned int nbloque
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int liberar_bloque(unsigned int nbloque) {
    struct superbloque SB;
    escribir_bit(nbloque, 0);
    bread(posSB,&SB);
    SB.cantBloquesLibres++;
    bwrite(posSB,&SB);

    return nbloque;
}


/*
    escribir_inodo: escribe el contenido de una variable de tipo struct inodo en un determinado inodo del array de inodos, inodos.
    input: unsigned int ninodo, struct inodo inodo
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo) {
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    bread(posSB,&SB);
    int bloqueInodo = SB.posPrimerBloqueAI + ninodo/(BLOCKSIZE/INODOSIZE);
    int indiceInodo = ninodo%(BLOCKSIZE/INODOSIZE);

    bread(bloqueInodo,inodos); // lectura del bloque de inodos para conservar el valor del resto de inodos
    inodos[indiceInodo] = inodo;
    return bwrite(bloqueInodo, inodos);
}

/*
    leer_inodo: lee un determinado inodo del array de inodos para volcarlo en una variable de tipo struct inodo pasada por referencia.
    input: unsigned int ninodo, struct inodo *inodo
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo) {
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    bread(posSB,&SB);
    int bloqueInodo = SB.posPrimerBloqueAI + ninodo/(BLOCKSIZE/INODOSIZE);
    int indiceInodo = ninodo%(BLOCKSIZE/INODOSIZE);

    bread(bloqueInodo,inodos); // lectura del bloque de inodos
    *inodo = inodos[indiceInodo];

    return 0; 
}

/*
    reservar_inodo: encuentra el primer inodo libre (dato almacenado en el superbloque), 
		lo reserva (con la ayuda de la función escribir_inodo()), devuelve su número y actualiza la lista enlazada de inodos libres.
    input: unsigned char tipo, unsigned char permisos
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos) {
    struct superbloque SB;
    bread(posSB,&SB);
    if (SB.cantInodosLibres != 0) {
        struct inodo inodo;
        unsigned int posInodoReservado = SB.posPrimerInodoLibre;

        // hacemos que el primer inodo libre sea el siguiente al que vamos a coger
        leer_inodo(posInodoReservado,&inodo);
        SB.posPrimerInodoLibre = inodo.punterosDirectos[0];


        // definimos el inodo con sus respectivos atributos
        inodo.permisos = permisos;
        inodo.tipo = tipo;
        inodo.nlinks = 1;
        inodo.tamEnBytesLog = 0;
        inodo.atime = inodo.ctime = inodo.mtime = time(NULL);
        inodo.numBloquesOcupados = 0;
        memset(inodo.punterosDirectos,0,sizeof(unsigned int)*12);
        memset(inodo.punterosIndirectos,0,sizeof(unsigned int)*3);

        // escribimos el inodo en la posición correspondiente
        escribir_inodo(posInodoReservado, inodo);
        SB.cantInodosLibres--; // reducimos el número de inodos libres en una unidad
        bwrite(posSB,&SB); // guardamos los cambios hechos al superbloque

        return posInodoReservado;
    }
    
    printf("Error: no hay inodos libres");
    return -1;
}

int obtener_nRangoBL(struct inodo *inodo,unsigned int nblogico,unsigned int *ptr) {
    if (nblogico<DIRECTOS) {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    } else if (nblogico<INDIRECTOS0) {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    } else if (nblogico<INDIRECTOS1) {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    } else if (nblogico<INDIRECTOS2) {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    } else {
        *ptr = 0;
        perror("Bloque lógico fuera de rango");
        return -1;
    }
}

int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros) {
    if (nblogico<DIRECTOS) {
        return nblogico;
    } else if (nblogico < INDIRECTOS0) {
        return nblogico-DIRECTOS;
    } else if (nblogico < INDIRECTOS1) {
        if (nivel_punteros == 2) {
            return (nblogico-INDIRECTOS0) / NPUNTEROS;
        } else if (nivel_punteros == 1) {
            return (nblogico-INDIRECTOS0) % NPUNTEROS;
        }
    } else if (nblogico < INDIRECTOS2) {
        if (nivel_punteros == 3) {
            return (nblogico-INDIRECTOS1) / (NPUNTEROS*NPUNTEROS);
        } else if (nivel_punteros == 2) {
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS)) / NPUNTEROS;
        } else if (nivel_punteros == 1) {
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS)) % NPUNTEROS;
        }
    }

    return -1;
}

/*
    traducir_bloque_inodo: se encarga de obtener el nº  de bloque físico correspondiente a un bloque lógico determinado del inodo indicado.
    input: unsigned int ninodo, unsigned int nblogico, char reservar
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar){
    struct inodo inodo;
    leer_inodo (ninodo, &inodo);
    unsigned int ptr = 0, ptr_ant = 0, nRangoBL, nivel_punteros, indice, salvar_inodo = 0; 
    unsigned int buffer[BLOCKSIZE / sizeof(unsigned int)];
    
    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr); //0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL;//el nivel_punteros +alto es el que cuelga del inodo
    
    //iterar para cada nivel de indirectos
    while (nivel_punteros > 0){
        //no cuelgan bloques de punteros
        if(!ptr){ 
            if(!reservar) {
                return -1; //error lectura bloque inexistente
            } else { //reservar bloques punteros y crear enlaces desde inodo hasta datos
                salvar_inodo = 1;
                ptr = reservar_bloque();
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL);
                if(nivel_punteros == nRangoBL){
                    inodo.punterosIndirectos[nRangoBL-1] = ptr;
                    fprintf(stderr,"traducir_bloque_inodo()->inodo.punterosIndirectos[%d] = %d (reservado BF %d para punteros_nivel%d)\n",nRangoBL-1,ptr,ptr,nivel_punteros);
                } else {
                    buffer[indice] = ptr;
                    fprintf(stderr,"traducir_bloque_inodo()->punteros_nivel%d[%d] = %d (reservado BF %d para punteros_nivel%d)\n",nivel_punteros+1,indice,ptr,ptr,nivel_punteros);
                    if (bwrite(ptr_ant,buffer) < 0) return -1;
                }
            }
        }
        if (bread(ptr,buffer) < 0) return -1;
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }
    
    if(!ptr){
        if(!reservar) {
            return -1;
        } else {
            salvar_inodo = 1;
            ptr = reservar_bloque();
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if(!nRangoBL){
                inodo.punterosDirectos[nblogico]=ptr;
                fprintf(stderr,"traducir_bloque_inodo()->inodo.punterosDirectos[%d] = %d (reservado BF %d para BL %d)\n",nblogico,ptr,ptr,nblogico);
            } else {
                buffer[indice] = ptr;
                fprintf(stderr,"traducir_bloque_inodo()->punteros_nivel%d[%d] = %d (reservado BF %d para BL %d)\n",nivel_punteros+1,indice,ptr,ptr,nblogico);
                if(bwrite(ptr_ant,buffer) < 0) return -1;
            }
        }
    }
    if(salvar_inodo) escribir_inodo(ninodo,inodo);

    return ptr;
}
  
