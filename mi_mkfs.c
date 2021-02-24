#include "bloques.h"

int main(int argc, char **argv) {
    int nbloques = atoi(argv[2]);

    if (bmount(argv[1]) != -1) {
        unsigned char *buf;
        memset(buf,0,BLOCKSIZE);
        for (unsigned int i = 0;i < nbloques;i++) {
            if (bwrite(i,buf) == -1) {
                return -1;
            }
        }

        return bumount();
    }
}