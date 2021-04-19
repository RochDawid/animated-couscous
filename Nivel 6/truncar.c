#include "ficheros.h"

int main(int argc,char **argv) {
    if (argv[1] && argv[2] && argv[3]) {
        int ninodo = atoi(argv[2]);
        int nbytes = atoi(argv[3]);
        bmount(argv[1]);

        if (nbytes == 0) {
            liberar_inodo(ninodo);
        } else {
            mi_truncar_f(ninodo,nbytes);
        }
        struct inodo inodo;
        leer_inodo(ninodo,&inodo);
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
          
        fprintf(stderr,"\nINODO : %d\n",ninodo);
        fprintf(stderr,"tipo=%c\n",inodo.tipo);
        fprintf(stderr,"permisos=%d\n",inodo.permisos);
        fprintf(stderr,"atime: %s\n",atime);
        fprintf(stderr,"ctime: %s\n",ctime);
        fprintf(stderr,"mtime: %s\n",mtime);
        fprintf(stderr,"nlinks=%d\n",inodo.nlinks);
        fprintf(stderr,"tamEnBytesLog=%d\n",inodo.tamEnBytesLog);
        fprintf(stderr,"numBloquesOcupados=%d\n",inodo.numBloquesOcupados);
        return bumount();
    }
    perror("Sintex error");
    return -1;
}