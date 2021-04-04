#include "ficheros.h"

int main(int argc,char **argv) { //VIGILAR SA SINTAXIS
    bmount("disco");
    unsigned int ninodo = reservar_inodo('f',6);
    unsigned int arrayOffset [] = {9000,209000,0,409605000,480000000};
    char buffer[BLOCKSIZE];
    strcpy(buffer, argv[1]);
    printf("strcpy : %s\n",buffer);
    int length = strlen(argv[1]);
    printf("length : %d\n",length);
    length = length * sizeof(char);
    printf("new length : %d\tsizeof : %ld\n",length,sizeof(char));
    int bytesEscritos = mi_write_f(ninodo,buffer,arrayOffset[2],length);
    printf("bytesEscritos : %d\n",bytesEscritos);
    char buffer_lec[BLOCKSIZE];
    int bytesLeidos = mi_read_f(ninodo,buffer_lec,arrayOffset[2],length);
    printf("bytesLeidos : %d\n",bytesLeidos);
    printf("Buffer : %s\n",buffer_lec);
    struct STAT state;
    mi_stat_f(ninodo,&state);
    bumount();
    return ninodo;
}