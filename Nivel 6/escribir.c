/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

int main(int argc,char **argv) { //VIGILAR SA SINTAXIS
    if (argv[1] && argv[2]) {
        bmount(argv[1]);
        int diferentes_inodos;
        if (argv[3]) {
            diferentes_inodos = atoi(argv[3]);
        } else {
            diferentes_inodos = 1;
        }
        unsigned int ninodo;
        if (diferentes_inodos == 0) { 
            ninodo = reservar_inodo('f',6);
        }
        //FILE *fichero;
        //fichero = fopen(argv[2],"r");
        unsigned int arrayOffset [] = {9000,209000,30725000,409605000,480000000};
        char buffer[BLOCKSIZE];
        int length;
        int i = 0;
        memset(buffer,0,BLOCKSIZE);
        
        fprintf(stderr,"antes\n");
        length = strlen(argv[2]);
        fprintf(stderr,"\nlongitud texto : %d\n",length);
        //strcpy(buffer, argv[2]);
        //memcpy(buffer,argv[2],BLOCKSIZE);
        
        //while (fgets(buffer,BLOCKSIZE,fichero) != NULL && i < 5) {
        while (i < 5) {
            //length = strlen(buffer);
            if (diferentes_inodos == 1) {
                ninodo = reservar_inodo('f',6);
            }
            fprintf(stderr,"\nNº inodo reservado : %d\n",ninodo);
            fprintf(stderr,"offset : %d\n",arrayOffset[i]);
            int bytesEscritos = mi_write_f(ninodo,argv[2],arrayOffset[i],length);
            fprintf(stderr,"BytesEscritos : %d\n",bytesEscritos);
            struct STAT state;
            mi_stat_f(ninodo, &state);
            fprintf(stderr,"stat.tamEnBytesLog = %d\n",state.tamEnBytesLog);
            fprintf(stderr,"stat.numBloquesOcupados = %d\n\n",state.numBloquesOcupados);
            i++;        
        }
        
        bumount();
        return ninodo;
    } else {
        perror("Error Sintax\n");
        return -1;
    }
}
