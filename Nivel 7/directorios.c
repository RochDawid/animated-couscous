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
    strcpy(inicial, token);

    while (token != NULL) {
        token = strtok(NULL, s);
        strcat(final, '/');
        strcat(final, token);
    }

    if (camino[strlen(camino)-1] == '/') ? tipo = 'd' : tipo = 'f';

    printf("token %s\n", token);
    printf("inicial %s\n", inicial);
    printf("final %s\n", final);
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
    struct entrada *entrada;
    struct inodo *inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    if (camino_parcial == "/") {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }

    extraer_camino(camino_parcial, inicial, final, &tipo);
    // controlar error

    leer_inodo(*p_inodo_dir, &inodo_dir);
    // controlar error permisos lectura

    // calcular cantidad de entradas que tiene el inodo
    num_entrada_inodo = 0;
    if (cant_entradas_inodo > 0) {
        // leer entrada
        while ((num_entrada_inodo < cant_entradas_inodo) && (inicial != entrada.nombre)) {
            num_entrada_inodo++;
            // leer siguiente entrada
        }
    }

    if (inicial != entrada.nombre) {
        switch(reservar) {
            case 0: // return ERROR_NO_EXISTE_ENTRADA_CONSULTA
            case 1: 
                if (inodo_dir->tipo == 'f') {
                    // return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
                }
                
        }
    }
}
