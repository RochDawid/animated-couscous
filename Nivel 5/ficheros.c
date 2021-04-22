/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

/*
    mi_write_f: Escribe el contenido procedente de un buffer de memoria, buf_original, de tamaño nbytes, en un fichero/directorio
    input: unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes
    output: 0
    uses: bread(),bwrite(), traducir_bloque_inodo(), leer_inodo(), memcpy(), leer_inodo(), escribir_inodo()
    used by: escribir.c
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) < 0) return -1;
    if ((inodo.permisos & 2) == 2) {
        unsigned int primerBL = offset/BLOCKSIZE;
        unsigned int ultimoBL = (offset + nbytes - 1)/BLOCKSIZE;
        unsigned int desp1 = offset%BLOCKSIZE;
        unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
        unsigned int nbfisico = traducir_bloque_inodo(ninodo,primerBL,1);
        unsigned char buf_bloque[BLOCKSIZE];
        unsigned int bytesEscritos;
        if (bread(nbfisico,buf_bloque) < 0) return -1;
        if (primerBL == ultimoBL) { // escribimos en un único bloque
            memcpy(buf_bloque + desp1, buf_original,nbytes);
            if (bwrite(nbfisico,buf_bloque) < 0) return -1;
            bytesEscritos = desp2 - desp1 + 1;
        } else { // tenemos que escribir en más de un bloque
            memcpy(buf_bloque + desp1, buf_original,BLOCKSIZE - desp1);
            bytesEscritos = bwrite(nbfisico,buf_bloque) - desp1;
            if (bytesEscritos < 0) return -1;
            for (int i = primerBL + 1;i < ultimoBL;i++) {
                nbfisico = traducir_bloque_inodo(ninodo,i,1);
                bytesEscritos += bwrite(nbfisico,buf_original + (BLOCKSIZE - desp1) + (i-primerBL-1)*BLOCKSIZE);
            }
            nbfisico = traducir_bloque_inodo(ninodo,ultimoBL,1);
            if (bread(nbfisico,buf_bloque) < 0) return -1;
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1),desp2 + 1);
            bytesEscritos += bwrite(nbfisico,buf_bloque) - (BLOCKSIZE - (desp2 + 1));
        }

        if (leer_inodo(ninodo,&inodo) < 0) return -1;
        time_t timer;
        if (offset > inodo.tamEnBytesLog || (offset + bytesEscritos) > inodo.tamEnBytesLog) {
            inodo.tamEnBytesLog = offset + bytesEscritos;
            inodo.ctime = time(&timer);
        }
        
        inodo.mtime = time(&timer);
        if (escribir_inodo(ninodo,inodo) < 0) return -1;
            
        return bytesEscritos;
    } else {
        perror("Error: no dispone de permisos para leer el fichero/directorio.");
        return -1;
    }
}

/*
    mi_read_f: Lee información de un fichero/directorio y la almacena en un buffer de memoria, buf_original
    input: unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes
    output: número de bytes leídos
    uses: bread(),bwrite(), leer_inodo(), traducir_bloque_inodo(), memcpy(), leer_inodo(), escribir_inodo()
    used by: leer.c
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    unsigned int bytesLeidos = 0;
    if (leer_inodo(ninodo, &inodo)==-1) return -1;
    if ((inodo.permisos & 4) == 4) {
        if (offset >= inodo.tamEnBytesLog) { // no podemos leer nada
            return bytesLeidos;
        }
        if ((offset + nbytes) >= inodo.tamEnBytesLog) { // pretende leer más allá de EOF
            nbytes = inodo.tamEnBytesLog-offset;
            // leemos sólo los bytes que podemos desde el offset hasta EOF
        }

        unsigned int primerBL = offset/BLOCKSIZE;
        unsigned int ultimoBL = (offset + nbytes - 1)/BLOCKSIZE;
        unsigned int desp1 = offset%BLOCKSIZE;
        unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
        unsigned int nbfisico = traducir_bloque_inodo(ninodo,primerBL,0);
        unsigned char buf_bloque[BLOCKSIZE];

        if (primerBL == ultimoBL) { // leemos de un único bloque
            if (nbfisico != -1) {
                if (bread(nbfisico, buf_bloque) == -1) return -1;
                memcpy(buf_original, buf_bloque + desp1, nbytes);
            }
            bytesLeidos = desp2 - desp1 + 1;
        } else { // tenemos que leer en más de un bloque
            if (nbfisico != -1) {
                if (bread(nbfisico, buf_bloque) == -1) return -1;
                memcpy(buf_original, buf_bloque + desp1,BLOCKSIZE - desp1);
            }
            bytesLeidos = BLOCKSIZE - desp1;
            for (int i = primerBL + 1;i < ultimoBL;i++) {
                nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                if (nbfisico != -1) {
                    if (bread(nbfisico, buf_bloque) == -1) return -1;
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (i-primerBL-1)*BLOCKSIZE, buf_bloque, BLOCKSIZE);
                }
                bytesLeidos += BLOCKSIZE;
            }
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
            if (nbfisico != -1) {
                if(bread(nbfisico, buf_bloque)==-1) return -1;
                memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque,desp2 + 1);
            }
            bytesLeidos += desp2 + 1;
        }
        
        if(leer_inodo(ninodo,&inodo)==-1) return -1;
        time_t timer;
        inodo.atime = time(&timer);
        if(escribir_inodo(ninodo,inodo)==-1) return -1;

    } else {
        perror("Error: no dispone de permisos para leer el fichero/directorio.");
    }
    return bytesLeidos;
}

/*
    mi_stat_f: Devuelve la metainformación de un fichero/directorio
    input: unsigned int ninodo, struct STAT *p_stat
    output: 0
    uses: bread(),bwrite()
    used by: mi_mkfs.c, leer_sf.c
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) < 0) return -1;
    p_stat->tipo = inodo.tipo;
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    p_stat->permisos = inodo.permisos;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;

    return 0;
}

/*
    mi_chmod_f: Cambia los permisos de un fichero/directorio con el valor que indique el argumento permisos
    input: unsigned int ninodo, unsigned char permisos
    output: 0
    uses: leer_inodo(), escribir_inodo()
    used by: permitir.c
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    struct inodo inodo;
    time_t timer;
    if (leer_inodo(ninodo, &inodo) < 0) return -1;
    inodo.permisos = permisos;
    inodo.ctime = time(&timer);
    if (escribir_inodo(ninodo, inodo) < 0) return -1;

    return 0;
}
