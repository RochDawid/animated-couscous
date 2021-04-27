/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

#define TAMNOMBRE 60 //tamaño del nombre de directorio o fichero, en ext2 = 256

struct entrada {
    char nombre[TAMNOMBRE];
    unsigned int ninodo;
}

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);