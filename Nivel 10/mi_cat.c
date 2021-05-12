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