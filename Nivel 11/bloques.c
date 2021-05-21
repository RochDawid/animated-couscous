/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "bloques.h"

static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

/*
    bmount: para montar el dispositivo virtual, y dado que se trata de un fichero, esa acción consistirá en abrirlo
    input: const char *camino
    output: int descriptor
    uses: umask(),open(),perror(), initSem()
    used by: mi_mkfs->main(), leer_sf(), leer.c, permitir.c truncar.c
*/
int bmount(const char *camino) {
    umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    
    if (descriptor == -1) {
        perror("Error montando el dispositivo virtual");
        return -1;
    }

    if (!mutex) {
        mutex = initSem();
        if (mutex == SEM_FAILED) {
            return -1;
        }
    }

    return descriptor;
}

/*
    bumount: desmonta el dispositivo virtual
    input: none
    output: 0 on success / -1 on failure
    uses: close(),perror(), deleteSem()
    used by: mi_mkfs->main(), leer_sf.c, leer.c, permitir.c, truncar.c
*/
int bumount() {
    if (close(descriptor) == -1) {
        perror("Error desmontando el dispositivo virtual");
        return -1;
    }

    deleteSem();
    
    return 0;
}

/*
    bwrite: escribe 1 bloque en el dispositivo virtual, en el bloque físico especificado por nbloque
    input: unsigned int nbloque, const void *buf
    output: BLOCKSIZE on success / -1 on failure
    uses: lseek(),write(),perror()
    used by: mi_mkfs() y muchas de las funciones de ficheros_basico.c usan esta función
*/
int bwrite(unsigned int nbloque, const void *buf) {
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if (lseek(descriptor,desplazamiento,SEEK_SET) != -1) {
        if (write(descriptor,buf,BLOCKSIZE) >= 0) {
            return BLOCKSIZE;
        }
    }

    perror("Error escribiendo en el dispositivo virtual");
    return -1;
}

/*
    bread: lee 1 bloque en el dispositivo virtual, en el bloque físico especificado por nbloque
    input: unsigned int nbloque, void *buf
    output: BLOCKSIZE on success / -1 on failure
    uses: lseek(),sizeof(),read(),perror()
    used by: mi_mkfs() y muchas de las funciones de ficheros_basico.c usan esta función
*/
int bread(unsigned int nbloque, void *buf) {
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if (lseek(descriptor,desplazamiento,SEEK_SET) != -1) {
        if (read(descriptor,buf,BLOCKSIZE) >= 0) {
            return BLOCKSIZE;
        }
    }

    perror("Error leyendo del dispositivo virtual");
    return -1;
}

void mi_waitSem() {
    if (!inside_sc) {
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}
