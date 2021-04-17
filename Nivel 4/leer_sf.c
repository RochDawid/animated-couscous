/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "ficheros_basico.h"

int main() {
    bmount("disco");
    struct superbloque sb;

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
    printf("\nSB.posPrimerInodoLibre : %d\n",sb.posPrimerInodoLibre);

/*    printf("\nsizeof struct superbloque: %d\n",(int) sizeof(sb));
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
    printf("totInodos = %d\n",sb.totInodos); */
    
    return bumount();
}