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
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 2) == 2) {
        unsigned int primerBL = offset/BLOCKSIZE;
        unsigned int ultimoBL = (offset + nbytes - 1)/BLOCKSIZE;
        unsigned int desp1 = offset%BLOCKSIZE;
        unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
        unsigned int nbfisico = traducir_bloque_inodo(ninodo,primerBL,1);
        //printf("1r BL : %d, ultBL : %d, desp1 : %d, desp2 : %d, nbfisico : %d, nbytes : %d\n",primerBL,ultimoBL,desp1,desp2,nbfisico,nbytes);
        unsigned char buf_bloque[BLOCKSIZE];
        unsigned int bytesEscritos;
        bread(nbfisico,buf_bloque);
        if (primerBL == ultimoBL) { // escribimos en un único bloque
            memcpy(buf_bloque + desp1, buf_original,nbytes);
            bwrite(nbfisico,buf_bloque);
            bytesEscritos = desp2 - desp1 + 1;
            //printf("mi_write -> bytesEscritos : %d\n",bytesEscritos);
        } else { // tenemos que escribir en más de un bloque
            memcpy(buf_bloque + desp1, buf_original,BLOCKSIZE - desp1);
            bytesEscritos = bwrite(nbfisico,buf_bloque) - desp1;
            for (int i = primerBL + 1;i < ultimoBL;i++) {
                nbfisico = traducir_bloque_inodo(ninodo,i,1);
                bytesEscritos += bwrite(nbfisico,buf_original + (BLOCKSIZE - desp1) + (i-primerBL-1)*BLOCKSIZE);
            }
            nbfisico = traducir_bloque_inodo(ninodo,ultimoBL,1);
            bread(nbfisico,buf_bloque);
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1),desp2 + 1);
            bytesEscritos += bwrite(nbfisico,buf_bloque) - (BLOCKSIZE - (desp2 + 1));
        }

        leer_inodo(ninodo,&inodo);
        time_t timer;
        if (offset > inodo.tamEnBytesLog || (offset + bytesEscritos) > inodo.tamEnBytesLog) {
            inodo.tamEnBytesLog = offset + bytesEscritos;
            inodo.ctime = time(&timer);
        }
        
        inodo.mtime = time(&timer);
        escribir_inodo(ninodo,inodo);
            
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
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    unsigned int bytesLeidos = 0;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 4) == 4) {
        //printf("mi_Read -> offset %d \t inodo.tamBytes : %d\n",offset,inodo.tamEnBytesLog);
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
                bread(nbfisico, buf_bloque);
                memcpy(buf_original, buf_bloque + desp1, nbytes);
            }
            bytesLeidos = desp2 - desp1 + 1;
        } else { // tenemos que leer en más de un bloque
            if (nbfisico != -1) {
                bread(nbfisico, buf_bloque);
                memcpy(buf_original, buf_bloque + desp1,BLOCKSIZE - desp1);
            }
            bytesLeidos = BLOCKSIZE - desp1;
            for (int i = primerBL + 1;i < ultimoBL;i++) {
                nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                if (nbfisico != -1) {
                    bread(nbfisico, buf_bloque);
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (i-primerBL-1)*BLOCKSIZE, buf_bloque, BLOCKSIZE);
                }
                bytesLeidos += BLOCKSIZE;
            }
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
            if (nbfisico != -1) {
                bread(nbfisico, buf_bloque);
                memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque,desp2 + 1);
            }
            bytesLeidos += desp2 + 1;
        }
        
        leer_inodo(ninodo,&inodo);
        time_t timer;
        inodo.atime = time(&timer);
        escribir_inodo(ninodo,inodo);

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
    used by: mi_mkfs(), leer_sf()
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
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
    uses: bread(),bwrite()
    used by: mi_mkfs(), leer_sf()
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    struct inodo inodo;
    time_t timer;
    leer_inodo(ninodo, &inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(&timer);
    escribir_inodo(ninodo, inodo);

    return 0;
}
