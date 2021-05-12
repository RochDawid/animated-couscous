/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

void mostrar_buscar_entrada(char *camino, char reservar){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    printf("\ncamino: %s, reservar: %d\n", camino, reservar);
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
        mostrar_error_buscar_entrada(error);
    }
    printf("**********************************************************************\n");
    return;
}

int main(int argc,char **argv) {
    if (argv[1]) {
        if (bmount(argv[1]) < 0) return -1;
        struct superbloque sb;
    
        if (bread(posSB,&sb) < 0) return -1;
        printf("\nDATOS DEL SUPERBLOQUE\n");
        printf("posPrimerBloqueMB = %d\n",sb.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",sb.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",sb.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",sb.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",sb.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",sb.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",sb.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",sb.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",sb.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",sb.cantInodosLibres);
        printf("totBloques = %d\n",sb.totBloques);
        printf("totInodos = %d\n\n",sb.totInodos);

        //Mostrar creación directorios y errores
        // mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
        // mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
        // mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
        // mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
        // mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
        // mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
        // mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1);  
        //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
        // mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
        // mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
        // mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
        // mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
        // mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2
        /* 
        reservar_inodo('f',6);
        printf("\nINODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 468.750\n");
        int bloquesLog[] = {8, 204, 30004, 400004, 468750};
        for (int i = 0;i < 5;i++) {
            traducir_bloque_inodo(1,bloquesLog[i],1);
        }
        
        struct inodo inodo;
        leer_inodo(1,&inodo);
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];

        ts = localtime(&inodo.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

        printf("\nDATOS DEL DIRECTORIO RAIZ\n");
        printf("tipo: %c\n",inodo.tipo);
        printf("permisos: %d\n",inodo.permisos);
        printf("atime: %s\n",atime);
        printf("ctime: %s\n",ctime);
        printf("mtime: %s\n",mtime);
        printf("nlinks: %d\n",inodo.nlinks);
        printf("tamEnBytesLog: %d\n",inodo.tamEnBytesLog);
        printf("numBloquesOcupados: %d\n",inodo.numBloquesOcupados);

        bread(posSB,&sb);
        printf("\nSB.posPrime rInodoLibre : %d\n",sb.posPrimerInodoLibre);

        printf("\nsizeof struct superbloque: %d\n",(int) sizeof(sb));
        printf("sizeof struct inodo: %d\n",(int) sizeof(struct inodo));

        printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");

        struct inodo arInodos[BLOCKSIZE/INODOSIZE];
        for (int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
            bread(i,arInodos);
            for (int j = 0; j < (BLOCKSIZE/INODOSIZE); j++) {
                if (arInodos[j].tipo == 'l') {
                    printf("%d ",arInodos[j].punterosDirectos[0]);
                }
            }
        }
        printf("\n");

        unsigned int nbloque = reservar_bloque();

        bread(posSB,&sb);
        printf("DATOS DEL SUPERBLOQUE\n");
        printf("posPrimerBloqueMB = %d\n",sb.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",sb.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",sb.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",sb.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",sb.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",sb.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",sb.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",sb.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",sb.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",sb.cantInodosLibres);
        printf("totBloques = %d\n",sb.totBloques);
        printf("totInodos = %d\n",sb.totInodos);

        liberar_bloque(nbloque);

        bread(posSB,&sb);
        printf("DATOS DEL SUPERBLOQUE\n");
        printf("posPrimerBloqueMB = %d\n",sb.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = %d\n",sb.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = %d\n",sb.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = %d\n",sb.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = %d\n",sb.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = %d\n",sb.posUltimoBloqueDatos);
        printf("posInodoRaiz = %d\n",sb.posInodoRaiz);
        printf("posPrimerInodoLibre = %d\n",sb.posPrimerInodoLibre);
        printf("cantBloquesLibres = %d\n",sb.cantBloquesLibres);
        printf("cantInodosLibres = %d\n",sb.cantInodosLibres);
        printf("totBloques = %d\n",sb.totBloques);
        printf("totInodos = %d\n",sb.totInodos); 
        */
        
        return bumount();
    }

    perror("Sintaxis: leer_sf <nombre_dispositivo>\n");
    return -1;
}
