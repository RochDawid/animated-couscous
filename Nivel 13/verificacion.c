/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"
#include "verificacion.h"

int main(int argc,char **argv) {
    if (argc != 2) {
        fprintf(stderr,"Sintaxis: ./verificacion <disco> <directorio_simulación>\n");
        return -1;
    }

    if (bmount(argv[1]) < 0) return -1;
    struct STAT stat;
    mi_stat(argv[2],stat); //ctr error
    int numentradas = stat.tamEnBytesLog/sizeof(struct entradas);
    if (numentradas != NUMPROCESOS) return -1;
    // char ruta
    strcat(argv[2],"informe.txt");
    mi_creat(argv[2],7);

    return bumount();
}
