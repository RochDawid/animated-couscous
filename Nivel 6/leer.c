#include "ficheros.h"

int main(int argc,char **argv) {
    if (argv[1]) {
        bmount(argv[1]);
        unsigned int ninodo = atoi(argv[2]);
        int offset = 0, leidos = 0, contadorLeidos = 0, tambuffer = 1500;
        char buffer[tambuffer];
        FILE *fichero;
        char string[128];

        if (argv[3] && argv[4]) {
            fichero = fopen(argv[4],"w");
        }
        memset(buffer,0,tambuffer);
        leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
        contadorLeidos = leidos;
        while (leidos > 0) {
            if (argv[3] && argv[4]) {
                fwrite(buffer,sizeof(char),sizeof(buffer),fichero);
            }
            offset += tambuffer;
            write(1,buffer,leidos);
            memset(buffer,0,tambuffer);
            leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
            
            contadorLeidos += leidos;
        }
        if (argv[3] && argv[4]) {
            fclose(fichero);
        }
        struct inodo inodo;
        leer_inodo(ninodo,&inodo);
        
        sprintf(string,"\nbytes leidos : %d\n",contadorLeidos);
        write(2,string,strlen(string));
        fprintf(stderr,"tamEnBytesLog : %d\n",inodo.tamEnBytesLog); 
        bumount();

        return ninodo;
    }

    perror("Sintax Error\n");
    return -1;
}