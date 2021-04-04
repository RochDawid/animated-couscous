#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 2) != 2) {
        unsigned int primerBL = offset/BLOCKSIZE;
        unsigned int ultimoBL = (offset + nbytes - 1)/BLOCKSIZE;
        unsigned int desp1 = offset%BLOCKSIZE;
        unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
        unsigned int nbfisico = traducir_bloque_inodo(ninodo,primerBL,1);
        unsigned char buf_bloque[BLOCKSIZE];
        unsigned int bytesEscritos;
        bread(nbfisico,buf_bloque);
        if (primerBL == ultimoBL) { // escribimos en un único bloque
            memcpy(buf_bloque + desp1, buf_original,nbytes);
            bytesEscritos = bwrite(nbfisico,buf_bloque) - desp1;
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
        if (bytesEscritos > inodo.tamEnBytesLog) {
            inodo.tamEnBytesLog = bytesEscritos;
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

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 4) != 4) {
        unsigned int leidos = 0;
        if (offset >= inodo.tamEnBytesLog) { // no podemos leer nada
            leidos = 0;
            return leidos;
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
        unsigned int bytesLeidos;

        if (primerBL == ultimoBL) { // escribimos en un único bloque
            if (nbfisico != -1) {
                bread(nbfisico,buf_bloque);
                memcpy(buf_original, buf_bloque + desp1,nbytes);
            }
            bytesLeidos = BLOCKSIZE - desp1;
        } else { // tenemos que escribir en más de un bloque
            if (nbfisico != -1) {
                bread(nbfisico,buf_bloque);
                memcpy(buf_original, buf_bloque + desp1,BLOCKSIZE - desp1);
            }
            bytesLeidos = BLOCKSIZE - desp1;
            for (int i = primerBL + 1;i < ultimoBL;i++) {
                nbfisico = traducir_bloque_inodo(ninodo,i,0);
                if (nbfisico != -1) {
                    bread(nbfisico,buf_bloque);
                    memcpy(buf_original, buf_bloque,BLOCKSIZE);
                }
                bytesLeidos += BLOCKSIZE;
            }
            nbfisico = traducir_bloque_inodo(ninodo,ultimoBL,1);
            if (nbfisico != -1) {
                bread(nbfisico,buf_bloque);
                memcpy(buf_original, buf_bloque + (nbytes - desp2 - 1),desp2 + 1);
            }
            bytesLeidos += desp2 + 1;
        }

        leer_inodo(ninodo,&inodo);
        time_t timer;
        inodo.atime = time(&timer);
        escribir_inodo(ninodo,inodo);
            
        return bytesLeidos;

    } else {
        perror("Error: no dispone de permisos para leer el fichero/directorio.");
        return -1;
    }
}

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

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    ts = localtime(&p_stat->atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat->mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat->ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

/*     printf("METAINFORMACIÓN DEL FICHERO/DIRECTORIO DEL INODO %d\n", ninodo);
    printf("tipo: %c\n",p_stat->tipo);
    printf("permisos: %d\n",p_stat->permisos);
    printf("atime: %ld\n",p_stat->atime);
    printf("ctime: %ld\n",p_stat->ctime);
    printf("mtime: %ld\n",p_stat->mtime);
    printf("nlinks: %d\n",p_stat->nlinks);
    printf("tamEnBytesLog: %d\n",p_stat->tamEnBytesLog);
    printf("numBloquesOcupados: %d\n",p_stat->numBloquesOcupados); */
    fprintf(stderr,"METAINFORMACIÓN DEL FICHERO/DIRECTORIO DEL INODO %d\n", ninodo);
    fprintf(stderr,"tipo: %c\n",p_stat->tipo);
    fprintf(stderr,"permisos: %d\n",p_stat->permisos);
    fprintf(stderr,"atime: %s\n",atime);
    fprintf(stderr,"ctime: %s\n",ctime);
    fprintf(stderr,"mtime: %s\n",mtime);
    fprintf(stderr,"nlinks: %d\n",p_stat->nlinks);
    fprintf(stderr,"tamEnBytesLog: %d\n",p_stat->tamEnBytesLog);
    fprintf(stderr,"numBloquesOcupados: %d\n",p_stat->numBloquesOcupados);

    return 0;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    struct inodo inodo;
    time_t timer;
    leer_inodo(ninodo, &inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(&timer);
    escribir_inodo(ninodo, inodo);

    return 0;
}
