/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

static struct UltimaEntrada UltimaEntradaEscritura[TAMFIFO];
static struct UltimaEntrada UltimaEntradaLectura[TAMFIFO];
static int numEntradaEscritura = 0;
static int numEntradaLectura = 0;

/*
    extraer_camino: función que pasándole una cadena de carácteres que indica el camino para acceder
                    al archivo que se desee lo separa en dos trozos (inicial y final) y determina el
                    tipo de archivo del que se trata (fichero o directorio).
    input: const char *camino, char *inicial, char *final, char *tipo
    output: 0 (success), -1 (failure)
    uses:
    used by: leer_sf.c
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    if (camino[0] == '/') {
        char dst[80];
        char *token;
        const char s[2] = "/"; // separador
        unsigned int nBarras = 0;

        strcpy(dst,camino);
        // contamos el número de barras del camino
        for (int i=0;dst[i]!='\0';i++) {
            if (dst[i] == '/') nBarras++;
        }
        strcpy(dst,camino+1); // omitimos la primera barra

        if (nBarras == 1) {
            strcpy(inicial,dst);
            *tipo = 'f';
            strcpy(final,"");
        } else {
            token = strchr(dst,'/');
            strcpy(final,token);
            token = strtok(dst, s);
            strcpy(inicial,token);

            if (final[0] == '/') {
                *tipo = 'd';
            } else {
                *tipo = 'f';
            }
        }
        return EXIT_SUCCESS;
    }
    return -1;
}

/*
    mostrar_error_buscar_entrada: muestra el error de la entrada según el entero pasado por parámetro.
    input: int error
    output: -
    uses: -
    used by: leer_sf.c
*/
void mostrar_error_buscar_entrada(int error) {
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

/*
    buscar_entrada: busca una determinada entrada entre las entradas del inodo correspondiente a su inodo padre.
    input: const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada,
           char reservar, unsigned char permisos
    output: 0 (success), -1 (failure)
    uses: -
    used by: leer_sf.c
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[TAMNOMBRE];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];

    if (!strcmp(camino_parcial,"/")) {
        struct superbloque SB;

        bread(posSB,&SB);
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }
    
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == -1) return ERROR_CAMINO_INCORRECTO;
    //fprintf(stderr,"buscar_entrada() -> inicial : %s, final : %s, reservar : %d\n",inicial,final,reservar);
    leer_inodo(*p_inodo_dir, &inodo_dir);
    if ((inodo_dir.permisos & 4) != 4) {
        //fprintf(stderr,"buscar_entrada() -> El inodo %d no tiene permisos de lectura\n",*p_inodo_dir);
        return ERROR_PERMISO_LECTURA;
    }

    // calcular cantidad de entradas que tiene el inodo
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    num_entrada_inodo = 0;
    int indice = 0;
    memset(entradas,0,BLOCKSIZE); // ponemos a 0 el buffer de lectura independientemente de la cantidad de entradas del inodo
    if (cant_entradas_inodo > 0) {
        int offset = 0;
        mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial,entradas[indice].nombre)) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && indice < BLOCKSIZE/sizeof(struct entrada) && strcmp(inicial,entradas[indice].nombre);num_entrada_inodo++,indice++) {}
            if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                indice = 0;
                offset += BLOCKSIZE;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
            } 
        }
    }

    if (strcmp(inicial,entradas[indice].nombre)) {
        switch(reservar) {
            case 0: return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1: 
                if (inodo_dir.tipo == 'f') {
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                if ((inodo_dir.permisos & 2) != 2) {
                    return ERROR_PERMISO_ESCRITURA;
                } else {
                    //copiar inicial en el nombre de la entrada
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
                    //fprintf(stderr,"buscar_entrada() -> Reservado inodo %d tipo %c con permisos %d para %s\n",entrada.ninodo,tipo,permisos,entrada.nombre);
                    if ((mi_write_f(*p_inodo_dir,&entrada,inodo_dir.tamEnBytesLog,sizeof(struct entrada))) == -1) {
                        if (entrada.ninodo != -1) { // si la escritura da error
                            liberar_inodo(entrada.ninodo); // liberamos el inodo
                        }
                        return -1;
                    }
                    //fprintf(stderr,"buscar_entrada() -> Creada entrada: %s, %d\n",entrada.nombre,entrada.ninodo);
                }
        }
    }

    if (!strcmp(final,"") || !strcmp(final,"/")) {
        if ((num_entrada_inodo < cant_entradas_inodo) && reservar == 1) {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        if (reservar==1) {
            *p_inodo = entrada.ninodo;
        } else {
            *p_inodo = entradas[indice].ninodo;
        }
        *p_entrada = num_entrada_inodo;

        return EXIT_SUCCESS;
    } else {
        *p_inodo_dir = entradas[indice].ninodo;

        return buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,permisos);
    }
    return EXIT_SUCCESS;
}

int mi_creat(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 1;
    int error;

    mi_waitSem();
    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return -1;
    }
    mi_signalSem();
    return 0;
}

int mi_dir(const char *camino, char *buffer, char tipo, int flag) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)], entradaFichero;
    struct inodo inodo, inodo_padre;
    int error;
    char tmp[80], numEntradas[10], fila[TAMFILA];
    struct tm *tm;
    char permisos[3];

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    leer_inodo(p_inodo, &inodo); // leemos el inodo de la entrada

    if (inodo.tipo != tipo) {
        fprintf(stderr,"Error: la sintaxis no concuerda con el tipo\n");
        return -1;
    }

    //if (inodo.tipo != 'd') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO; // no se trata de un directorio
    if ((inodo.permisos & 4) != 4) return ERROR_PERMISO_LECTURA; // no tiene permisos de lectura

    int cant_entradas_inodo;
    if (tipo == 'd') {
        // calcular cantidad de entradas que tiene el inodo
        cant_entradas_inodo = inodo.tamEnBytesLog/sizeof(struct entrada);

        sprintf(numEntradas,"Total: %d\n",cant_entradas_inodo);
        strcpy(buffer,numEntradas);
    }

    if (!flag) {
        strcat(buffer,"Tipo    Permisos    mTime                   Tamaño            Nombre      \n");
        strcat(buffer,"--------------------------------------------------------------------------\n");
    } else {
        strcat(buffer,"Nombre    \n");
        strcat(buffer,"----------\n");
    }

    if (tipo == 'f') {
        leer_inodo(p_inodo_dir,&inodo_padre);

        mi_read_f(p_inodo_dir,&entradaFichero,p_entrada*sizeof(struct entrada),sizeof(struct entrada));
        if (flag) {
            strcat(buffer,entradaFichero.nombre);
            return 0;
        }
        if (inodo.permisos & 4) strcpy(permisos, "r"); else strcpy(permisos, "-");
        if (inodo.permisos & 2) strcat(permisos, "w"); else strcat(permisos, "-");
        if (inodo.permisos & 1) strcat(permisos, "x"); else strcat(permisos, "-");
        tm = localtime(&inodo.mtime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);

        sprintf(fila,"%c       %s         %s     %-9d         %s\n",inodo.tipo,permisos,tmp,inodo.tamEnBytesLog,entradaFichero.nombre);
        strcat(buffer,fila);
        
        return 0;
    }

    int num_entrada_inodo = 0;
    int indice = 0;
    struct inodo inodo_iterado;
    memset(entradas,0,BLOCKSIZE);
    if (cant_entradas_inodo > 0) {
        int offset = 0;
        mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
        while (num_entrada_inodo < cant_entradas_inodo) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && indice < BLOCKSIZE/sizeof(struct entrada);num_entrada_inodo++,indice++) {
                leer_inodo(entradas[indice].ninodo,&inodo_iterado);

                if (!flag) {
                    memset(fila,0,TAMFILA);

                    if (inodo_iterado.permisos & 4) strcpy(permisos, "r"); else strcpy(permisos, "-");
                    if (inodo_iterado.permisos & 2) strcat(permisos, "w"); else strcat(permisos, "-");
                    if (inodo_iterado.permisos & 1) strcat(permisos, "x"); else strcat(permisos, "-");
                    tm = localtime(&inodo_iterado.mtime);
                    sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,  tm->tm_sec);

                    if (inodo_iterado.tipo == 'd') sprintf(fila,"\033[31m%c       %s         %s     %-9d         %s\033[0m\n",inodo_iterado.tipo,permisos,tmp,inodo_iterado.tamEnBytesLog,entradas[indice].nombre);
                    else sprintf(fila,"%c       %s         %s     %-9d         %s\n",inodo_iterado.tipo,permisos,tmp,inodo_iterado.tamEnBytesLog,entradas[indice].nombre);
                    strcat(buffer,fila);
                } else {
                    strcat(buffer,entradas[indice].nombre);
                    strcat(buffer,"\n");
                }
            }
            if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                indice = 0;
                offset += BLOCKSIZE;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
            } 
        }
    }

    return num_entrada_inodo;
}

int mi_chmod(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }
    mi_chmod_f(p_inodo, permisos);
    return EXIT_SUCCESS;
}

int mi_stat(const char *camino, struct STAT *p_stat) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }
    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    int iterador = 0;
    while (iterador < TAMFIFO) {
        if (strcmp(UltimaEntradaEscritura[iterador].camino,camino) == 0) {
            p_inodo = UltimaEntradaEscritura[iterador].p_inodo;
            //fprintf(stderr,"\nmi_write() -> Utilizamos la caché de escritura en vez de llamar a buscar_entrada()\n");
            iterador = TAMFIFO;
        }
        iterador++;
    }

    if (iterador == TAMFIFO) {
        if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
            mostrar_error_buscar_entrada(error);
            return 0;
        }
        UltimaEntradaEscritura[numEntradaEscritura].p_inodo = p_inodo;
        memset(UltimaEntradaEscritura[numEntradaEscritura].camino,0,sizeof(UltimaEntradaEscritura[numEntradaEscritura].camino));
        strcpy(UltimaEntradaEscritura[numEntradaEscritura].camino,camino);
        //fprintf(stderr,"mi_write() -> Actualizamos la caché de escritura\n");
        if (numEntradaEscritura == TAMFIFO-1) {
            numEntradaEscritura = 0;
        } else {
            numEntradaEscritura++;
        }
    }

    return mi_write_f(p_inodo, buf, offset, nbytes);
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    int iterador = 0;
    while (iterador < TAMFIFO) {
        if (strcmp(UltimaEntradaLectura[iterador].camino,camino) == 0) {
            p_inodo = UltimaEntradaLectura[iterador].p_inodo;
            //fprintf(stderr,"\nmi_read() -> Utilizamos la caché de lectura en vez de llamar a buscar_entrada()\n");
            iterador = TAMFIFO;
        }
        iterador++;
    }

    if (iterador == TAMFIFO) {
        if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
            mostrar_error_buscar_entrada(error);
            return 0;
        }
        UltimaEntradaLectura[numEntradaLectura].p_inodo = p_inodo;
        memset(UltimaEntradaLectura[numEntradaLectura].camino,0,sizeof(UltimaEntradaLectura[numEntradaLectura].camino));
        strcpy(UltimaEntradaLectura[numEntradaLectura].camino,camino);
        //fprintf(stderr,"mi_read() -> Actualizamos la caché de lectura\n");
        if (numEntradaLectura == TAMFIFO-1) {
            numEntradaLectura = 0;
        } else {
            numEntradaLectura++;
        }
    }

    return mi_read_f(p_inodo, buf, offset, nbytes);
}

/*
    mi_link: crea el enlace de una entrada de directorio camino2 al inodo especificado
             por otra entrada de directorio camino1
    input: const char *camino1, const char *camino2
    output: 0 (success), -1 (failure)
    uses: -
    used by: mi_link.c
*/
int mi_link(const char *camino1, const char *camino2) {
    mi_waitSem();
    unsigned int p_inodo_dir1 = 0, p_inodo_dir2 = 0;
    unsigned int p_inodo1 = 0, p_inodo2 = 0;
    unsigned int p_entrada1 = 0, p_entrada2 = 0;
    char reservar = 0;
    int error;
    struct inodo inodo1;

    // comprobamos que la entrada camino1 existe
    if ((error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, reservar, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return -1;
    }

    leer_inodo(p_inodo1, &inodo1); // leemos el inodo de la entrada
    // comprobamos que tiene permisos de lectura
    if ((inodo1.permisos & 4) != 4) {
        fprintf(stderr,"El inodo %d no tiene permisos de lectura\n",p_inodo1);
        mi_signalSem();
        return -1;
    }

    reservar = 1;
    // si no existe la entrada camino2
    if ((error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, reservar, 6)) == 0) {
        struct entrada entrada;
        mi_read_f(p_inodo_dir2, &entrada, sizeof(struct entrada) * p_entrada2, sizeof(struct entrada));
        entrada.ninodo = p_inodo1; // hacemos el enlace asignándole el inodo de la primera entrada a la segunda
        // escribimos la entrada modificada en p_inodo_dir2
        mi_write_f(p_inodo_dir2, &entrada, sizeof(struct entrada) * p_entrada2, sizeof(struct entrada));
        liberar_inodo(p_inodo2);
        inodo1.nlinks++; // incrementamos la cantidad de enlaces de p_inodo1
        time_t timer;
        inodo1.ctime = time(&timer);      // actualizamos el ctime
        escribir_inodo(p_inodo1, inodo1); // salvamos el inodo
        mi_signalSem();
        return 0;
    } else {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return -1;
    }
}

/*
    mi_unlink: Función de la capa de directorios que borra la entrada de directorio especificada (no hay que olvidar 
            actualizar la cantidad de enlaces en el inodo) y, en caso de que fuera el último enlace existente, 
            borrar el propio fichero/directorio.
    input: const char *camino
    output: 0 (success), -1 (failure)
    uses: 
    used by: mi_rm.c
*/
int mi_unlink(const char *camino) {
    mi_waitSem();
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;
    struct inodo inodo, inodo_dir;

    // comprobamos que la entrada camino existe
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return -1;
    }

    leer_inodo(p_inodo, &inodo);

    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0) {
        fprintf(stderr,"Error: El directorio %s no está vacío\n",camino);
        mi_signalSem();
        return -1;
    } 

    leer_inodo(p_inodo_dir, &inodo_dir);
    int nEntradas = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    if (p_entrada != nEntradas-1) {
        struct entrada entrada;
        mi_read_f(p_inodo_dir,&entrada,inodo_dir.tamEnBytesLog-sizeof(struct entrada),sizeof(struct entrada));
        mi_write_f(p_inodo_dir,&entrada,sizeof(struct entrada)*p_entrada,sizeof(struct entrada));
    }
    mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog-sizeof(struct entrada));

    leer_inodo(p_inodo, &inodo);
    inodo.nlinks--;
    if (inodo.nlinks == 0) {
        liberar_inodo(p_inodo);
        mi_signalSem();
        return 1;
    } else {
        time_t timer;
        inodo.ctime = time(&timer); // actualizamos el ctime
        escribir_inodo(p_inodo,inodo);
        mi_signalSem();
        return 0;
    }
}

/*
    mi_unlink_r: Función de la capa de directorios que borra las entradas de directorios no vacíos por debajo 
            de la entrada especificada (no hay que olvidar actualizar la cantidad de enlaces en el inodo) y, 
            en caso de que fuera el último enlace existente, borrar el propio fichero/directorio.
    input: const char *camino, int ninodo
    output: 0 (success), -1 (failure)
    uses: mi_unlink(), mi_unlink_r()
    used by: mi_rm_r.c
*/
int mi_unlink_r(const char *camino, int ninodo) {
    struct inodo inodo;
    char camino_aux[strlen(camino) + 25];

    memset(camino_aux,0,strlen(camino_aux));
    strcpy(camino_aux,camino);

    leer_inodo(ninodo, &inodo);

    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    // calcular cantidad de entradas que tiene el inodo
    int cant_entradas_inodo;
    if (inodo.tipo == 'd') cant_entradas_inodo = inodo.tamEnBytesLog/sizeof(struct entrada);
    else cant_entradas_inodo = 0;
    int num_entrada_inodo = 0;
    int indice = 0;
    memset(entradas,0,BLOCKSIZE); // ponemos a 0 el buffer de lectura independientemente de la cantidad de entradas del inodo
    struct inodo inodo_entrada;
    if (cant_entradas_inodo > 0) {
        int offset = 0;
        mi_read_f(ninodo,entradas,offset,BLOCKSIZE);
        while (num_entrada_inodo < cant_entradas_inodo) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && indice < BLOCKSIZE/sizeof(struct entrada);num_entrada_inodo++,indice++) {
                leer_inodo(entradas[indice].ninodo,&inodo_entrada);
                strcat(camino_aux,entradas[indice].nombre);
                if (inodo_entrada.tipo == 'd') {
                    strcat(camino_aux,"/");
                }
                mi_unlink_r(camino_aux, entradas[indice].ninodo);
                memset(camino_aux,0,strlen(camino_aux));
                //fprintf(stderr,"1 %s\n",camino);
                strcpy(camino_aux,camino);
            }
            if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                indice = 0;
                offset += BLOCKSIZE;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(ninodo,entradas,offset,BLOCKSIZE);
            } 
        }
    }

    mi_unlink(camino);
    return 0;
}
