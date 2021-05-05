/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    if (camino[0] == '/') {
        char *dst;
        char *token;
        const char s[2] = "/";

        strcpy(dst,camino);

        token = strchr(dst,'/');
        strcpy(final,token);

        // comprovar quins mètodes són necessaris
        inicial = strtok(dst, s);
        strcpy(inicial,token);

        if (final[0] == '/') {
            *tipo = 'd';
        } else {
            *tipo = 'f';
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
    struct entrada entrada;
    struct inodo inodo_dir;
    //char inicial[sizeof(entrada.nombre)];
    char inicial[TAMNOMBRE];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;
    //unsigned char buffer[BLOCKSIZE];
    //unsigned int punteros[BLOCKSIZE/sizeof(unsigned int)];
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    fprintf(stderr,"hola1\n");
    if (!strcmp(camino_parcial,"/")) {
        struct superbloque SB;
        
        bread(posSB,&SB);
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }
    fprintf(stderr,"hola2\n");
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == 1) return ERROR_CAMINO_INCORRECTO;
    fprintf(stderr,"hola\n");

    leer_inodo(*p_inodo_dir, &inodo_dir);
    if ((inodo_dir.permisos & 4) != 4) return ERROR_PERMISO_LECTURA;

    // calcular cantidad de entradas que tiene el inodo
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    num_entrada_inodo = 0;
    if (cant_entradas_inodo > 0) {
        // leer entrada
        //for (int i = 0;i < strlen(inodo_dir->punterosDirectos) && strcmp(inicial,entradas[num_entrada_inodo].nombre);i++) {
            int offset = 0;
            memset(entradas,offset,BLOCKSIZE);
            //bread(inodo_dir->punterosDirectos[i],entradas);
            int modulo = 0;
            mi_read_f(*p_inodo_dir,entradas,0,BLOCKSIZE);
            //while ((num_entrada_inodo < cant_entradas_inodo) && (inicial != entrada.nombre)) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[num_entrada_inodo].nombre);num_entrada_inodo++,modulo++) {
                
                modulo %= BLOCKSIZE/sizeof(struct entrada);
                if (modulo == 0) {
                    memset(entradas,0,BLOCKSIZE);
                    offset += BLOCKSIZE;
                    mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
                }
            }
        //}

/*         for (int i = 0;i < strlen(inodo_dir->punterosIndirectos) && strcmp(inicial,entradas[num_entrada_inodo].nombre);i++) {
            memset(entradas,0,strlen(entradas));
            bread(inodo_dir->punterosIndirectos[i],punteros);
            for (int j = 0;(j < strlen(punteros)) && strcmp(inicial,entradas[num_entrada_inodo].nombre);j++;) {
                memset(punteros,0,strlen(punteros));
                bread(punteros[j],entradas);
                for (;(num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[num_entrada_inodo].nombre);num_entrada_inodo++;) {
                }
            }
        } */
    }

    if (strcmp(inicial,entradas[num_entrada_inodo].nombre)) {
        switch(reservar) {
            case 0: return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1: 
                if (inodo_dir.tipo == 'f') {
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                if ((inodo_dir.permisos & 2) != 2) {
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                } else {
                    //copiar *inicial en el nombre de la entrada
                    strcpy(entrada.nombre,inicial);
                    if (tipo == 'd') {
                        if (!strcmp(final,"/")) {
                            entrada.ninodo = reservar_inodo('d',permisos);
                        } else {
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    } else {
                        entrada.ninodo = reservar_inodo('f',permisos);
                    }
                    strcpy(entrada.nombre,entradas[num_entrada_inodo].nombre);
                    
                    entrada.ninodo = entradas[num_entrada_inodo].ninodo;
                    if ((mi_write_f(*p_inodo_dir,&entrada,num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada))) == -1) {
                        if (entrada.ninodo != -1) {
                            liberar_inodo(entrada.ninodo);
                        }
                        return EXIT_FAILURE;
                    }
                }
        }
    }

    if (!strcmp(final,"")) {
        if ((num_entrada_inodo < cant_entradas_inodo) && reservar == 1) {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        //
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;

        return EXIT_SUCCESS;
    } else {
        //
        *p_inodo_dir = entrada.ninodo;

        return buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,permisos);
    }
    return EXIT_SUCCESS;
}
