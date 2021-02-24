#include "bloques.h"

int descriptor;
//test
int bmount(const char *camino) {
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    
    if (descriptor == -1) {
        // error
        perror("Error!");
        return -1;
    }

    return descriptor;
}

int bumount() {
    if (close(descriptor) == -1) {
        // error
        printf("Error!");
        return -1;
    }
    
    return 0;
}

int bwrite(unsigned int nbloque, const void *buf) {
    off_t desplazamiento = nbloque * BLOCKSIZE;
    if (lseek(descriptor,desplazamiento,SEEK_SET) != -1) {
        unsigned int nbytes = (unsigned int)sizeof(*buf);
        if (write(descriptor,buf,nbytes) >= 0) {
            return BLOCKSIZE;
        }
    }
    
    return -1;
}

