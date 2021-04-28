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
    token = strtok(camino, s);

    while (token != NULL) {
        token = strtok(NULL, s);
        strcat(final, token);
    }

    if (camino[strlen(camino)] == '/') ? tipo = 'd' : tipo = 'f';

    strcpy(inicial, token);
    printf("token %s\n", token);
    printf("inicial %s\n", inicial);
    printf("final %s\n", final);
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {

}
