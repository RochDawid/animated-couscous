/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    if (camino[0] == '/') {
        char *dst = (camino+1);
        char *token;
        const char s[2] = "/";

        token = strchr(dst,'/');
        strcpy(final,token);

        // comprovar quins mètodes són necessaris
        token = strtok(dst, s);
        strcpy(inicial,token);

        

        if (final[0] == '/') {
            tipo = 'd';
        } else {
            tipo = 'f';
        }

        return EXIT_SUCCESS;

        printf("token %s\n", token);
        printf("inicial %s\n", inicial);
        printf("final %s\n", final);
    }

    return EXIT_FAILURE;
}

void mostrar_error_buscar_entrada(int error) {
   // fprintf(stderr, "Error: %d\n", error);
    switch (error) {
        case -1: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
        case -2: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
        case -3: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
        case -4: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
        case -5: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
        case -6: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
        case -7: fprintf(stderr, "Error: No es un directorio.\n"); break;
    }
}


int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
    struct entrada *entrada;
    struct inodo *inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;
    unsigned char buffer[BLOCKSIZE];
    unsigned int punteros[BLOCKSIZE/sizeof(unsigned int)];
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];

    if (camino_parcial == "/") {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == 1) return ERROR_CAMINO_INCORRECTO;


    leer_inodo(*p_inodo_dir, &inodo_dir);
    if ((inodo_dir->permisos & 4) != 4) return ERROR_PERMISO_LECTURA;

    // calcular cantidad de entradas que tiene el inodo
    cant_entradas_inodo = sizeof(struct inodo)/sizeof(struct entrada);
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
