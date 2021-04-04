#include "ficheros.h"

int main(int argc, char **argv) {
    bmount("disco");
    unsigned int ninodo = reservar_inodo('f',6);
    unsigned int arrayOffset = {9000,209000,30725000,409605000,480000000};
    const char buffer[BLOCKSIZE];
    strcpy(buffer, "lo que siga");
    int length = strlen("lo que siga");
    mi_write_f(ninodo,buffer,arrayOffset,length);
    memset(buffer,0,BLOCKSIZE);
    mi_read_f(ninodo,buffer,arrayOffset,length);
    printf("Buffer : %s\n",buffer);
    struct STAT *state;
    mi_stat_f(ninodo,state);
    return bumount();
}