#include "bloques.h"

static int descriptor = 0;

/*
    bmount: para montar el dispositivo virtual, y dado que se trata de un fichero, esa acción consistirá en abrirlo
    input: const char *camino
    output: int descriptor
    uses: umask(),open(),perror()
    used by: mi_mkfs->main()
*/
int bmount(const char *camino) {
    umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    
    if (descriptor == -1) {
        // error
        perror("Error!");
        return -1;
    }

    return descriptor;
}

/*
    bumount: desmonta el dispositivo virtual
    input: none
    output: 0 on success / -1 on failure
    uses: close(),perror()
    used by: mi_mkfs->main()
*/
int bumount() {
    if (close(descriptor) == -1) {
        // error
        perror("Error!");
        return -1;
    }
    
    return 0;
}

/*
    bwrite: escribe 1 bloque en el dispositivo virtual, en el bloque físico especificado por nbloque
    input: unsigned int nbloque, const void *buf
    output: BLOCKSIZE on success / -1 on failure
    uses: lseek(),write(),perror()
    used by: mi_mkfs->main()
*/
int bwrite(unsigned int nbloque, const void *buf) {
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if (lseek(descriptor,desplazamiento,SEEK_SET) != -1) {
        if (write(descriptor,buf,BLOCKSIZE) >= 0) {
            return BLOCKSIZE;
        }
    }

    perror("Error!");
    return -1;
}

/*
    bread: lee 1 bloque en el dispositivo virtual, en el bloque físico especificado por nbloque
    input: unsigned int nbloque, void *buf
    output: BLOCKSIZE on success / -1 on failure
    uses: lseek(),sizeof(),read(),perror()
    used by:
*/
int bread(unsigned int nbloque, void *buf) {
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if (lseek(descriptor,desplazamiento,SEEK_SET) != -1) {
        if (read(descriptor,buf,nbytes) >= 0) {
            return BLOCKSIZE;
        }
    }

    perror("Error!");
    return -1;
}

