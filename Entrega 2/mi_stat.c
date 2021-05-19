/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    // comprobamos la sintaxis
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_stat <disco> </ruta>\n");
    }
    if (bmount(argv[1]) < 0) return -1;
    struct STAT state;
    unsigned int ninodo = mi_stat(argv[2],&state);
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    ts = localtime(&state.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&state.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&state.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    
    fprintf(stderr,"\nINODO : %d\n",ninodo);
    fprintf(stderr,"tipo=%c\n",state.tipo);
    fprintf(stderr,"permisos=%d\n",state.permisos);
    fprintf(stderr,"atime: %s\n",atime);
    fprintf(stderr,"ctime: %s\n",ctime);
    fprintf(stderr,"mtime: %s\n",mtime);
    fprintf(stderr,"nlinks=%d\n",state.nlinks);
    fprintf(stderr,"tamEnBytesLog=%d\n",state.tamEnBytesLog);
    fprintf(stderr,"numBloquesOcupados=%d\n",state.numBloquesOcupados);

    return bumount();
}