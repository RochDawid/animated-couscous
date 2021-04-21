/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "ficheros_basico.h"

int main() {
    if (bmount("disco") < 0) return -1;
    struct superbloque sb;

    if (bread(posSB,&sb) < 0) return -1;
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
    
    unsigned int nbloque = reservar_bloque();
    if (nbloque == -1) return -1;
    if (bread(posSB,&sb) < 0) return -1;
    printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    printf("Se ha reservado el bloque físico nº %d que era el 1º libre indicado por el MB\n",nbloque);
    printf("SB.cantBloquesLibres = %d\n",sb.cantBloquesLibres);
    liberar_bloque(nbloque);
    if (bread(posSB,&sb) < 0) return -1;
    printf("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n",sb.cantBloquesLibres);

    int testBits[] = {posSB,sb.posPrimerBloqueMB,sb.posUltimoBloqueMB,sb.posPrimerBloqueAI,sb.posUltimoBloqueAI,sb.posPrimerBloqueDatos,sb.posUltimoBloqueDatos};
    printf("\nMAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    for (int i = 0;i < 7;i++) {
        printf("leer_bit(%d)= %d\n",testBits[i],leer_bit(testBits[i]));
    }

    struct inodo inodo;
    if (leer_inodo(sb.posInodoRaiz,&inodo) < 0) return -1;
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
    
    return bumount();
}
