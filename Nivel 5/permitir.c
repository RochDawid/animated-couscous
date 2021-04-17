/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros.h"

int main(int argc,char **argv) {
    if (argc == 4) {
        bmount(argv[1]);
        mi_chmod_f(argv[2],argv[3]);    
        return bumount();
    }
    perror("Sintaxis incorrecta\n");
    return -1;
}
