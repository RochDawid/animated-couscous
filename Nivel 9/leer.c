/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    /* if (argv[1]) {
        if (bmount(argv[1]) < 0) return -1;
        unsigned int ninodo = atoi(argv[2]);
        int offset = 0, leidos = 0, contadorLeidos = 0, tambuffer = 1500;
        char buffer[tambuffer];
        FILE *fichero;
        char string[128];

        if (argv[3] && argv[4]) {
            fichero = fopen(argv[4],"w");
        }
        // leemos el fichero y acumulamos todos lo bytes leidos hasta el final
        memset(buffer,0,tambuffer);
        leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
        if (leidos == -1) return -1;
        contadorLeidos = leidos;
        while (leidos > 0) {
            if (argv[3] && argv[4]) {
                fwrite(buffer,sizeof(char),sizeof(buffer),fichero);
            }
            offset += tambuffer;
            write(1,buffer,leidos);
            memset(buffer,0,tambuffer);
            leidos = mi_read_f(ninodo,buffer,offset,tambuffer);
            if (leidos == -1) return -1;
            contadorLeidos += leidos;
        }
        if (argv[3] && argv[4]) {
            fclose(fichero);
        }
        struct inodo inodo;
        if (leer_inodo(ninodo,&inodo) < 0) return -1;
        
        sprintf(string,"\nbytes leidos : %d\n",contadorLeidos);
        write(2,string,strlen(string));
        fprintf(stderr,"tamEnBytesLog : %d\n",inodo.tamEnBytesLog); 
        if (bumount() < 0) return -1;

        return ninodo;
    }

    perror("Sintaxis: leer <nombre_dispositivo>\n");
    return -1; */
    //Comprobamos sintaxis
    if (argc != 5){
        fprintf(stderr, "Sintaxis: ./leer <disco> </ruta_fichero> \n");
        exit(-1);
    }
    int offset = 0, leidos = 0, contadorLeidos = 0, tambuffer = 1500;
    char buffer[tambuffer];

    //struct STAT stat;
    //montamos el dispositivo
    if (bmount(argv[1]) < 0) return -1;


    //obtenemos la ruta y comprobamos que no se refiera a un directorio
    if (argv[2][strlen(argv[2]) - 1] == '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un directorio");
        exit(-1);
    }
    char *camino = argv[2];
    // escribimos varias veces el texto desplazado 1 bloque
    memset(buffer,0,tambuffer);
        leidos = mi_read(camino,buffer,offset,tambuffer);
        if (leidos == -1) return -1;
        contadorLeidos = leidos;

        while (leidos > 0) {
            offset += tambuffer;
            leidos += mi_read(camino, buffer, offset, tambuffer);
            memset(buffer,0,tambuffer);
            
        }
    fprintf(stderr, "Bytes leidos: %d\n", leidos);
    /* Visualización del stat
    mi_stat_f(ninodo, &stat);
    printf("stat.tamEnBytesLog=%d\n",stat.tamEnBytesLog);
    printf("stat.numBloquesOcupados=%d\n",stat.numBloquesOcupados);
    */

    return bumount();
}
