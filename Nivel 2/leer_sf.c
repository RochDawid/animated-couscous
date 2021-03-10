#include "ficheros_basico.h"

int main() {
    int descriptor = bmount("disco");
    struct superbloque sb;
    
    if (descriptor != -1) {
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

        printf("sizeof struct superbloque: %d\n",(int) sizeof(sb));
        printf("sizeof struct inodo: %d\n",(int) sizeof(struct inodo));

        printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");

        struct inodo arInodos[BLOCKSIZE/INODOSIZE];
        for (int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
            if (bread(i,arInodos) == -1) {
                perror("Error leyendo");
                return -1;
            }
            for (int j = 0; j <= (BLOCKSIZE/INODOSIZE); j++) {
                printf("%d\n",arInodos[j].punterosDirectos[0]);
            }
        }
        bumount();
    } else {
        perror("Error montando dispositivo virtual");
    }
}