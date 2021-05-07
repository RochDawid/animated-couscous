/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Error. Sintaxis: ./mi_stat <disco> </ruta>\n");
    }
    if (bmount(argv[1]) < 0) return -1;
    struct STAT *p_stat;
    mi_stat(argv[2],p_stat);
    return bumount();
}