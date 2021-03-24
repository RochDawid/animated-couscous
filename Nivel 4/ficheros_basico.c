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
    //int nbloqueMB = posbyte / BLOCKSIZE;
    //int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

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
        printf("Posición del primer bloque libre de inodos pre: %d\n", posInodoReservado);

        // hacemos que el primer inodo libre sea el siguiente al que vamos a coger
        leer_inodo(posInodoReservado,&inodo);
        SB.posPrimerInodoLibre = inodo.punterosDirectos[0];
        printf("Posición del primer bloque libre de inodos post: %d\n", SB.posPrimerInodoLibre);


        // definimos el inodo con sus respectivos atributos
        inodo.permisos = permisos;
        inodo.tipo = tipo;
        inodo.nlinks = 1;
        inodo.tamEnBytesLog = 0;
        inodo.atime = inodo.ctime = inodo.mtime = (time_t) NULL;
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
    
}

int obtener_indice(int nblogico, int nivel_punteros) {
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
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar){
    unsigned int ptr = 0;
    // leemos el inodo solicitado.
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if (nblogico < DIRECTOS){ // el bloque logico es uno de los 12 primeros bloques logicos del inodo.
        switch (reservar){
            case 0:// modo consulta
                if (inodo.punterosDirectos[nblogico] == 0) {// no tiene bloque físico asignado.
                    return -1;
                } else {
                   ptr=inodo.punterosDirectos[nblogico];
                }
                break;
            case 1:// modo escritura
                if (inodo.punterosDirectos[nblogico] == 0) {// si no tiene bloque fisico le asignamos uno.
                    inodo.punterosDirectos[nblogico] = reservar_bloque();
                    ptr=inodo.punterosDirectos[nblogico];
                    // aumentamos en uno el numero de bloques ocupados por el inodo en la zona de datos:
                    inodo.numBloquesOcupados++;
                    inodo.ctime = time(NULL);
                    // escribimos el inodo con la info actualizada
                    escribir_inodo(ninodo,inodo);
                } else { // tiene bloque fisico asignado y lo devolvemos
                    ptr=inodo.punterosDirectos[nblogico];
                }
                break;
        }
        //printf("nblogico= %d, ptr= %d\n", nblogico, ptr);
        return ptr;
    }
    // PUNTERO INDIRECTOS 0
    // El bloque logico lo encontramos en el rango de Indirectos 0, es decir, 
    // está comprendido entre   el 0+12 y el 0+12+256-1: entre el 12 y el 267 
    else if (nblogico < INDIRECTOS0) {
        unsigned int punteros_nivel1[NPUNTEROS];
        switch (reservar){
            case 0:// modo consulta
                if (inodo.punterosIndirectos[0] == 0) { // no hay bloque fisico asignado a punteros_nivel1.
                    return -1;
                } else { // ya existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                    bread(inodo.punterosIndirectos[0],punteros_nivel1);
                    if (punteros_nivel1[nblogico-DIRECTOS] == 0) {
                    // no hay bloque físico asignado al bloque lógico de datos.
                        return -1;
                    } else {
                        ptr=punteros_nivel1[nblogico-DIRECTOS];
                    }
                }
                break;
            case 1:// modo escritura
                if (inodo.punterosIndirectos[0] == 0){
                // no hay bloque fisico asignado a punteros_nivel1, asi que creamos un buffer con 0s,  y
                // reservamos un bloque donde salvarlo.
                    memset(punteros_nivel1,0,BLOCKSIZE);//iniciamos a 0 los 256 punteros.
                    inodo.punterosIndirectos[0] = reservar_bloque();
                    // reservamos un nuevo bloque para los datos y anotamos su nº 
                    // en el índice correspondiente de punteros_nivel1 
                    punteros_nivel1[nblogico-DIRECTOS] = reservar_bloque();
                    // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                    inodo.numBloquesOcupados+=2;
                    inodo.ctime = time(NULL);
                    bwrite(inodo.punterosIndirectos[0],punteros_nivel1);
                    // devolvemos el bloque fisico de datos
                    ptr=punteros_nivel1[nblogico-DIRECTOS];
                } else { // existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                    bread(inodo.punterosIndirectos[0],punteros_nivel1);
                    if (punteros_nivel1[nblogico-DIRECTOS] == 0){
                        // no hay bloque fisico de datos asignado, entonces lo reservamos
                        punteros_nivel1[nblogico-DIRECTOS] = reservar_bloque();
                        // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                        inodo.numBloquesOcupados++;
                        inodo.ctime = time(NULL);
                        // salvamos el bloque de punteros_nivel1 en el dispositivo.
                        bwrite(inodo.punterosIndirectos[0],punteros_nivel1);
                        ptr=punteros_nivel1[nblogico-DIRECTOS]; // devolvemos el bloque fisico de datos.
                    } else { // si existe el bloque fisico de datos lo devolvemos
                        ptr=punteros_nivel1[nblogico-DIRECTOS];
                    }
                }
                // escribimos en el dispositivo el inodo actualizado
                escribir_inodo(ninodo,inodo);
                break;
        }
        //printf("nblogico= %d, ptr= %d\n", nblogico, ptr);
        return ptr;
    }
    // PUNTERO INDIRECTOS 1
    // El bloque logico lo encontramos en el rango de Indirectos 1, es decir, 
    // los comprendidos entre el 0+12+256 y el 0+12+256+256^2-1: entre el 268 y el 65.803.
    else if (nblogico < INDIRECTOS1){
        unsigned int punteros_nivel1[NPUNTEROS];
        unsigned int punteros_nivel2[NPUNTEROS];
        unsigned int indice_nivel1 = obtener_indice(nblogico, 1); // indice para punteros_nivel1
        unsigned int indice_nivel2 = obtener_indice(nblogico, 2); // indice para punteros_nivel2
        switch(reservar) {
            case 0: // modo consulta
                if (inodo.punterosIndirectos[1] == 0) { // no hay bloque fisico asignado a punteros_nivel2.
                    return -1;
                } else { // ya existe el bloque de punteros_nivel2 y lo leemos del dispositivo
                    bread(inodo.punterosIndirectos[1],punteros_nivel2);
                    if (punteros_nivel2[indice_nivel2] == 0) { // no hay bloque fisico asignado a punteros_nivel1.
                        return -1;
                    } else { // ya existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                        bread(punteros_nivel2[indice_nivel2],punteros_nivel1);
                        if (punteros_nivel1[indice_nivel1] == 0) {
                            // no hay bloque físico asignado al bloque lógico de datos.
                            return -1;
                        } else {
                            ptr=punteros_nivel1[indice_nivel1]; // devolvemos el bloque fisico solicitado
                        }
                    }
                }
                break;
            case 1:// modo escritura
                if (inodo.punterosIndirectos[1] == 0){
                // no hay bloque fisico asignado a punteros_nivel2, asi que llenamos un buffer con 0s,  y
                // reservamos un bloque donde salvarlo. 
                    memset(punteros_nivel2,0,BLOCKSIZE);
                    inodo.punterosIndirectos[1] = reservar_bloque(); 
                    // llenamos el buffer de punteros_nivel1 con 0s y reservamos un bloque para él, 
                    // anotamos su nº  en el índice correspondiente de punteros_nivel2 
                    memset(punteros_nivel1,0,BLOCKSIZE);
                    punteros_nivel2[indice_nivel2] = reservar_bloque(); 
                    // reservamos un nuevo bloque para los datos y anotamos su nº 
                    // en el índice correspondiente de punteros_nivel1 
                    punteros_nivel1[indice_nivel1] = reservar_bloque(); 
                    // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                    inodo.numBloquesOcupados+=3;
                    inodo.ctime = time(NULL);
                    // salvamos los buffers de los bloques de punteros en el dispositivo
                    bwrite(inodo.punterosIndirectos[1],punteros_nivel2);
                    bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                    // devolvemos el bloque fisico de datos
                    ptr=punteros_nivel1[indice_nivel1];
                } else { // existe el bloque de punteros_nivel2 y lo leemos del dispositivo
                   bread(inodo.punterosIndirectos[1],punteros_nivel2);
                   if (punteros_nivel2[indice_nivel2] == 0) { 
                        // no hay bloque fisico de punteros_nivel1 asignado, entonces lo reservamos y
                        // anotamos su nº  en el índice correspondiente de punteros_nivel2
                        memset(punteros_nivel1,0,BLOCKSIZE);
                        punteros_nivel2[indice_nivel2] = reservar_bloque();   
                        // reservamos un nuevo bloque para los datos y anotamos su nº 
                        // en el índice correspondiente de punteros_nivel1 
                        punteros_nivel1[indice_nivel1] = reservar_bloque(); 
                        // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                        inodo.numBloquesOcupados+=2;
                        inodo.ctime = time(NULL);
                        // salvamos los buffers de los bloques de punteros en el dispositivo
                        bwrite(inodo.punterosIndirectos[1],punteros_nivel2);
                        bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                        // devolvemos el bloque fisico de datos
                        ptr=punteros_nivel1[indice_nivel1];
                    } else { // existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                        bread(punteros_nivel2[indice_nivel2],punteros_nivel1);    
                        if (punteros_nivel1[indice_nivel1] == 0) {
                            // no hay bloque físico asignado al bloque lógico de datos así que lo reservamos 
                            // y anotamos su nº en el índice correspondiente de punteros_nivel1.
                            punteros_nivel1[indice_nivel1] = reservar_bloque();
                            // aumentamos en uno el numero de bloques ocupados por el inodo en la zona de datos.
                            inodo.numBloquesOcupados++;
                            inodo.ctime = time(NULL);
                            // salvamos el bloque de punteros_nivel1 en el dispositivo
                            bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                            // devolvemos el bloque físico asignado a los datos
                            ptr = punteros_nivel1[indice_nivel1];
                        } else {
                            ptr = punteros_nivel1[indice_nivel1];
                        }
                    }
                }
            // escribimos en el dispositivo el inodo actualizado
            escribir_inodo(ninodo,inodo);
            break;
        }
        //printf("nblogico= %d, ptr= %d\n", nblogico, ptr);
        return ptr;
    }
    // PUNTERO INDIRECTOS 2
    // El bloque logico lo encontramos en el rango de Indirectos 2, es decir, los comprendidos entre 
    // el 0+12+256+256^2 y el 0+12+256+256^2+256^3-1: entre el 65.804 y el 16.843.019.
    else if (nblogico < INDIRECTOS2){
        unsigned int punteros_nivel1[NPUNTEROS];
        unsigned int punteros_nivel2[NPUNTEROS];
        unsigned int punteros_nivel3[NPUNTEROS];
        unsigned int indice_nivel1 = obtener_indice(nblogico, 1); //indice para punteros_nivel1
        unsigned int indice_nivel2 = obtener_indice(nblogico, 2); //indice para punteros_nivel2
        unsigned int indice_nivel3 = obtener_indice(nblogico, 3); //indice para punteros_nivel3
        switch(reservar){
            case 0: //modo consulta
                if (inodo.punterosIndirectos[2] == 0) { // no hay bloque fisico asignado a punteros_nivel3.
                    return -1;
                } else { // ya existe el bloque de punteros_nivel3 y lo leemos del dispositivo
                    bread(inodo.punterosIndirectos[2],punteros_nivel3);
                    if (punteros_nivel3[indice_nivel3] == 0) { // no hay bloque fisico asignado a punteros_nivel2.
                    return -1;
                    } else { // ya existe el bloque de punteros_nivel2 y lo leemos del dispositivo
                        bread(punteros_nivel3[indice_nivel3],punteros_nivel2);
                        if (punteros_nivel2[indice_nivel2] == 0) { // no hay bloque fisico asignado a punteros_nivel1.
                            return -1;
                        } else { // ya existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                            bread(punteros_nivel2[indice_nivel2],punteros_nivel1);
                            if (punteros_nivel1[indice_nivel1] == 0) {
                                // no hay bloque físico asignado al bloque lógico de datos.
                                return -1;
                            } else {
                                ptr=punteros_nivel1[indice_nivel1]; // devolvemos el bloque fisico solicitado
                            }
                        }
                    }
                }
                break;
            case 1: //modo escritura
                if (inodo.punterosIndirectos[2] == 0) {               
                    // no hay bloque fisico asignado a punteros_nivel3, asi que llenamos un buffer con 0s,  y
                    // reservamos un bloque donde salvarlo. 
                    memset(punteros_nivel3,0,BLOCKSIZE);
                    inodo.punterosIndirectos[2] = reservar_bloque();
                    // llenamos el buffer de punteros_nivel2 con 0s y reservamos un bloque para él, 
                    //anotamos su nº  en el índice correspondiente de punteros_nivel3 
                    memset(punteros_nivel2,0,BLOCKSIZE);
                    punteros_nivel3[indice_nivel3] = reservar_bloque();
                    // llenamos el buffer de punteros_nivel1 con 0s y reservamos un bloque para él, 
                    //anotamos su nº  en el índice correspondiente de punteros_nivel2 
                    memset(punteros_nivel1,0,BLOCKSIZE);
                    punteros_nivel2[indice_nivel2] = reservar_bloque();
                    // reservamos un nuevo bloque para los datos y anotamos su nº 
                    // en el índice correspondiente de punteros_nivel1 
                    punteros_nivel1[indice_nivel1] = reservar_bloque();
                    // aumentamos en uno el numero de bloques ocupados por el inodo en la zona de datos.
                    inodo.numBloquesOcupados+=4;
                    inodo.ctime = time(NULL);
                    // salvamos los buffers de los bloques de punteros en el dispositivo
                    bwrite(inodo.punterosIndirectos[2],punteros_nivel3);
                    bwrite(punteros_nivel3[indice_nivel3],punteros_nivel2);
                    bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                    // devolvemos el bloque fisico de datos
                    ptr=punteros_nivel1[indice_nivel1];
                } else { // existe el bloque de punteros_nivel3 y lo leemos del dispositivo
                    bread(inodo.punterosIndirectos[2],punteros_nivel3);
                    if (punteros_nivel3[indice_nivel3] == 0) {
                        // no hay bloque fisico de punteros_nivel2 asignado, entonces lo reservamos y
                        // anotamos su nº  en el índice correspondiente de punteros_nivel3
                        memset(punteros_nivel2,0,BLOCKSIZE);
                        punteros_nivel3[indice_nivel3] = reservar_bloque();     
                        // llenamos el buffer de punteros_nivel1 con 0s y reservamos un bloque para él, 
                        // anotamos su nº  en el índice correspondiente de punteros_nivel2 
                        memset(punteros_nivel1,0,BLOCKSIZE);
                        punteros_nivel2[indice_nivel2] = reservar_bloque(); 
                        // reservamos un nuevo bloque para los datos y anotamos su nº 
                        // en el índice correspondiente de punteros_nivel1 
                        punteros_nivel1[indice_nivel1] = reservar_bloque(); 
                        // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                        inodo.numBloquesOcupados+=3;
                        inodo.ctime = time(NULL);
                        // salvamos los buffers de los bloques de punteros en el dispositivo
                        bwrite(inodo.punterosIndirectos[2],punteros_nivel3);
                        bwrite(punteros_nivel3[indice_nivel3],punteros_nivel2);
                        bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                        // devolvemos el bloque fisico de datos
                        ptr=punteros_nivel1[indice_nivel1];
                    } else { // existe el bloque de punteros_nivel2 y lo leemos del dispositivo
                        bread(punteros_nivel3[indice_nivel3],punteros_nivel2);
                        if (punteros_nivel2[indice_nivel2] == 0) {
                            // no hay bloque fisico de punteros_nivel1 asignado, entonces lo reservamos y
                            // anotamos su nº  en el índice correspondiente de punteros_nivel2
                            memset(punteros_nivel1,0,BLOCKSIZE);
                            punteros_nivel2[indice_nivel2] = reservar_bloque(); 
                            // reservamos un nuevo bloque para los datos y anotamos su nº 
                            // en el índice correspondiente de punteros_nivel1 
                            punteros_nivel1[indice_nivel1] = reservar_bloque();                            
                            // aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                            inodo.numBloquesOcupados+=2;
                            inodo.ctime = time(NULL);
                            // salvamos los buffers de los bloques de punteros en el dispositivo
                            bwrite(punteros_nivel3[indice_nivel3],punteros_nivel2);
                            bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                            // devolvemos el bloque fisico de datos
                            ptr=punteros_nivel1[indice_nivel1];
                        } else { // existe el bloque de punteros_nivel1 y lo leemos del dispositivo
                            bread(punteros_nivel2[indice_nivel2],punteros_nivel1);
                            if (punteros_nivel1[indice_nivel1] == 0){
                                // no hay bloque físico asignado al bloque lógico de datos así que lo reservamos 
                                // y anotamos su nº en el índice correspondiente de punteros_nivel1.
                                punteros_nivel1[indice_nivel1] = reservar_bloque();                                
                                //aumentamos el numero de bloques ocupados por el inodo en la zona de datos.
                                inodo.numBloquesOcupados++;
                                inodo.ctime = time(NULL);
                                // salvamos el bloque de punteros_nivel1 en el dispositivo
                                bwrite(punteros_nivel2[indice_nivel2],punteros_nivel1);
                                // devolvemos el bloque físico asignado a los datos
                                ptr=punteros_nivel1[indice_nivel1];
                            } else {
                                ptr=punteros_nivel1[indice_nivel1];
                            }
                        }
                    }
                }
                // escribimos en el dispositivo el inodo actualizado
                escribir_inodo(ninodo,inodo);
                break;
        }
        //printf("nblogico= %d, ptr= %d\n", nblogico, ptr);
        return ptr;
    }
    return ptr;
}
