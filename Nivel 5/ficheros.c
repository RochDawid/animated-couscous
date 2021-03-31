#include "ficheros_basico.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {

}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);
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

        return leidos;
    } else {
        perror("Error: no dispone de permisos para leer el fichero/directorio.");
    }
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat) {
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);
    p_stat->tipo = inodo->tipo;
    p_stat->atime = inodo->atime;
    p_stat->ctime = inodo->ctime;
    p_stat->mtime = inodo->mtime;
    p_stat->nlinks = inodo->nlinks;
    p_stat->numBloquesOcupados = inodo->numBloquesOcupados;
    p_stat->permisos = inodo->permisos;
    p_stat->tamEnBytesLog = inodo->tamEnBytesLog;

    printf("METAINFORMACIÓN DEL FICHERO/DIRECTORIO DEL INODO %d\n", ninodo);
    printf("tipo: %c\n",p_stat->tipo);
    printf("permisos: %d\n",p_stat->permisos);
    printf("atime: %s\n",p_stat->atime);
    printf("ctime: %s\n",p_stat->ctime);
    printf("mtime: %s\n",p_stat->mtime);
    printf("nlinks: %d\n",p_stat->nlinks);
    printf("tamEnBytesLog: %d\n",p_stat->tamEnBytesLog);
    printf("numBloquesOcupados: %d\n",p_stat->numBloquesOcupados);

    return 0;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos) {
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);
    inodo->permisos = permisos;
    inodo->ctime = time(NULL);
    escribir_inodo(ninodo, inodo);

    return 0;
}
