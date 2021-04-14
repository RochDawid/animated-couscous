#include "ficheros.h"

int main(int argc,char **argv) { //VIGILAR SA SINTAXIS
    if (argv[1]) {
        bmount(argv[1]);
        //unsigned int ninodo = reservar_inodo('f',6);
        unsigned int ninodo = atoi(argv[2]);
        fprintf(stderr,"ninodo : %d\n",ninodo);
        int offset = 0, tambuffer = 1500;
        char buffer[tambuffer];
        FILE *fichero;
        char string[128];

        fprintf(stderr, "Abans fopen");
        fichero = fopen(argv[4],"w");
        fprintf(stderr, "Després fopen");
        memset(buffer,0,tambuffer);
        fprintf(stderr, "Abans mi_read_f");
        int leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
        fprintf(stderr, "Després mi_read_f");
        while (leidos > 0) {
            //fputs(buffer,fichero);
            fwrite(buffer,sizeof(char),sizeof(buffer),fichero);
            write(1,buffer,leidos);
            offset += tambuffer;
            memset(buffer,0,tambuffer);
            leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
            //leidos == tamEnByteLog
        }
        fclose(fichero);
        /* struct inodo inodo;
        leer_inodo(ninodo,&inodo);
        printf("Número bytes leídos : %d\n",numBytesLeidos);
        printf("tamEnBytesLog : %d\n",inodo.tamEnBytesLog); */
        sprintf(string,"bytes leidos : %d\n",leidos);
        write(2,string,strlen(string));
        bumount();

        return ninodo;
    }

    perror("Sintax Error\n");
    return -1;
}