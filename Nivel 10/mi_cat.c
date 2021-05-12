/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3){
        fprintf(stderr, "Sintaxis: ./leer <disco> </ruta_fichero> \n");
        exit(-1);
    }
    int offset = 0, leidos = 0, contadorLeidos = 0, tambuffer = BLOCKSIZE*4; //PROBAR tambuffer=1500
    char buffer[tambuffer];
    char string[128];

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

    while (leidos > 0) {
        write(1,buffer,leidos);
        contadorLeidos += leidos;
        offset += tambuffer;
        leidos += mi_read(camino, buffer, offset, tambuffer);
        memset(buffer,0,tambuffer);        
    }
    sprintf(string,"\nbytes leidos : %d\n",contadorLeidos);
    write(2,string,strlen(string));
    /* Visualización del stat
    mi_stat_f(ninodo, &stat);
    printf("stat.tamEnBytesLog=%d\n",stat.tamEnBytesLog);
    printf("stat.numBloquesOcupados=%d\n",stat.numBloquesOcupados);
    */
    return bumount();
}