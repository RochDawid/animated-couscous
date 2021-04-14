#include "ficheros.h"

int main(int argc,char **argv) { //VIGILAR SA SINTAXIS
    if (argv[1] && argv[2]) {
        bmount(argv[1]);
        int diferentes_inodos = atoi(argv[3]);
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
        
        strcpy(buffer, argv[2]);
        length = strlen(argv[2]);
        printf("\nlongitud texto : %d\n",length);
        //while (fgets(buffer,BLOCKSIZE,fichero) != NULL && i < 5) {
        while (i < 5) {
            //length = strlen(buffer);
            if (diferentes_inodos == 1) {
                ninodo = reservar_inodo('f',6);
            }
            printf("\nNº inodo reservado : %d\n",ninodo);
            printf("offset : %d\n",arrayOffset[i]);
            int bytesEscritos = mi_write_f(ninodo,buffer,arrayOffset[i],length);
            printf("BytesEscritos : %d\n",bytesEscritos);
            struct STAT state;
            mi_stat_f(ninodo, &state);
            printf("stat.tamEnBytesLog = %d\n",state.tamEnBytesLog);
            printf("stat.numBloquesOcupados = %d\n\n",state.numBloquesOcupados);
            //memset(buffer,0,BLOCKSIZE);
            //int bytesLeidos = mi_read_f(ninodo,buffer,arrayOffset[i],length);
            //printf("bytesEscritos : %d\tbytesLeidos : %d\n",bytesEscritos,bytesLeidos);
            //puts(buffer);
            
            //write(1,buffer,b);
            //printf("Buffer : %s\n",buffer);
            //memset(buffer,0,BLOCKSIZE);
            i++;        
        }
        /* if (fichero == NULL || fgets(buffer,50,fichero) == NULL) {
            printf("Null\n");
        }
        */
        
        /* printf("length : %d\n",length);
        length = length * sizeof(char);
        printf("new length : %d\tsizeof : %ld\n",length,sizeof(char));
        int bytesEscritos = mi_write_f(ninodo,argv[1],arrayOffset[2],length); */
        
        //fclose(fichero);
        // printf("bytesEscritos : %d\n",bytesEscritos);

    /*     int bytesLeidos = mi_read_f(ninodo,buffer,arrayOffset[2],length);
        printf("bytesLeidos : %d\n",bytesLeidos);
        printf("Buffer : %s\n",buffer);*/
        
        bumount();
        return ninodo;
    } else {
        perror("Error Sintax\n");
        return -1;
    }
}