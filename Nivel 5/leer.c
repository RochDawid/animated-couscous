#include "ficheros.h"

int main(int argc,char **argv) { //VIGILAR SA SINTAXIS
    if (argv[1] != NULL) {
        bmount(argv[1]);
        //unsigned int ninodo = reservar_inodo('f',6);
        unsigned int ninodo = atoi(argv[2]);
        //unsigned int arrayOffset [] = {9000,209000,30725000,409605000,480000000};
        char buffer[BLOCKSIZE];
        int offset = 0,numBytesLeidos = 0, tambuffer = 1500;
        FILE *fichero;
        char string[128];

        fichero = fopen(argv[4],"w");

        memset(buffer,0,tambuffer);
        int leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
        while (leidos > 0) {
            fputs(buffer,fichero);
            numBytesLeidos += write(1,buffer,leidos);
            offset += tambuffer;
            memset(buffer,0,tambuffer);
            leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
            //leidos == tamEnByteLog
        }
        fclose(ninodo);
        struct inodo inodo;
        leer_inodo(ninodo,&inodo);
        printf("Número bytes leídos : %d\n",numBytesLeidos);
        printf("tamEnBytesLog : %d\n",inodo.tamEnBytesLog);
        write(2,string,strlen(string));
        bumount();
        return ninodo;
    } else {
        perror("Sintax Error\n");
        return -1;
    }
}