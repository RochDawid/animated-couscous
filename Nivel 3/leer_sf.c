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

    printf("\nsizeof struct superbloque: %d\n",(int) sizeof(sb));
    printf("sizeof struct inodo: %d\n",(int) sizeof(struct inodo));

/*     printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");

    struct inodo arInodos[BLOCKSIZE/INODOSIZE];
    for (int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
        bread(i,arInodos);
        for (int j = 0; j < (BLOCKSIZE/INODOSIZE); j++) {
            if (arInodos[j].tipo == 'l') {
                printf("%d ",arInodos[j].punterosDirectos[0]);
            }
        }
    }
    printf("\n"); */


    unsigned int nbloque = reservar_bloque();
    bread(posSB,&sb);
    printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    printf("Se ha reservado el bloque físico nº %d que era el 1º libre indicado por el MB\n",nbloque);
    printf("SB.cantBloquesLibres = %d\n",sb.cantBloquesLibres);
    liberar_bloque(nbloque);
    bread(posSB,&sb);
    printf("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n",sb.cantBloquesLibres);

    struct inodo inodo;
    leer_inodo(0,&inodo);
    printf("\nDATOS DEL DIRECTORIO RAIZ\n");
    printf("tipo: %c\n",inodo.tipo);
    printf("permisos: %c\n",inodo.permisos);
    printf("atime: \n");
    printf("ctime: \n");
    printf("mtime: \n");
    printf("nlinks: %d\n",inodo.nlinks);
    printf("tamEnBytesLog: %d\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n",inodo.numBloquesOcupados);
    
    return bumount();
}