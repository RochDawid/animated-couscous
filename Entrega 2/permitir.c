/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

int main(int argc,char **argv) {
    // comprobamos la sintaxis
    if (argc == 4) {
        bmount(argv[1]);
        mi_chmod_f(atoi(argv[2]),atoi(argv[3])); // cambiamos los permisos   
        return bumount();
    }
    
    fprintf(stderr,"Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
    return -1;
}
