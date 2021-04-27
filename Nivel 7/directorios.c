/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    char *token;
    const char s[2] = "/";

    // comprovar quins mètodes són necessaris
    token = strtok(camino,s);

    strcpy(inicial,token);
    printf("token %s\n",token);
    printf("inicial %s\n",inicial);
}