#include "ficheros.h"

int main() {
    bmount("disco");
    unsigned int ninodo = reservar_inodo('f',6);
    unsigned int arrayOffset [] = {9000,209000,30725000,409605000,480000000};
    char buffer[BLOCKSIZE];
    strcpy(buffer, "lo que siga");
    int length = strlen("lo que siga");
    mi_write_f(ninodo,buffer,arrayOffset[0],length);
    char buffer_lec[BLOCKSIZE];
    mi_read_f(ninodo,buffer_lec,arrayOffset[0],length);
    printf("Buffer : %s\n",buffer_lec);
    struct STAT state;
    mi_stat_f(ninodo,&state);
    return bumount();
}