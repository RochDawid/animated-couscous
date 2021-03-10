#include "ficheros_basico.h"

int main() {
    int descriptor = bmount("disco");
    struct superbloque sb;
    struct inodo in;
    void *buf;
    
    if (descriptor != -1) {
        for (int i=0;i<tamSB; i++) {
            bread(i,buf);
        }
        // sb = buf;
        printf("DATOS DEL SUPERBLOQUE");
        printf("posPrimerBloqueMB = ",sb.posPrimerBloqueMB);
        printf("posUltimoBloqueMB = ",sb.posUltimoBloqueMB);
        printf("posPrimerBloqueAI = ",sb.posPrimerBloqueAI);
        printf("posUltimoBloqueAI = ",sb.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos = ",sb.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos = ",sb.posUltimoBloqueDatos);
        printf("posInodoRaiz = ",sb.posInodoRaiz);
        printf("posPrimerInodoLibre = ",sb.posPrimerInodoLibre);
        printf("cantBloquesLibres = ",sb.cantBloquesLibres);
        printf("cantInodosLibres = ",sb.cantInodosLibres);
        printf("totBloques = ",sb.totBloques);
        printf("totInodos = ",sb.totInodos);

        printf("sizeof struct superbloque: ",sizeof(sb));
        printf("sizeof struct inodo: ",sizeof(in));

        printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES");

    } else {
        perror("Error montando dispositivo virtual");
    }
}