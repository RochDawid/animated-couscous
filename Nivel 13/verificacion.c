/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "directorios.h"
#include "verificacion.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./verificacion <disco> <directorio_simulación>\n");
        return -1;
    }

    if (bmount(argv[1]) < 0) return -1;
    struct STAT stat;
    mi_stat(argv[2],&stat); //ctr error
    int numentradas = stat.tamEnBytesLog/sizeof(struct entrada);
    if (numentradas != NUMPROCESOS) return -1;
    char ruta_informe[80];
    strcpy(ruta_informe,argv[2]);
    strcat(ruta_informe,"informe.txt");
    mi_creat(ruta_informe,7);
    
    struct entrada buffer_entradas[NUMPROCESOS*sizeof(struct entrada)];
    int offset = 0;
    memset(buffer_entradas,0,sizeof(buffer_entradas));
    mi_read(argv[2],buffer_entradas,offset,sizeof(buffer_entradas));
    for (int indice = 0;indice < sizeof(buffer_entradas);indice++) {
        struct INFORMACION info;
        char *p_pid = strchr(buffer_entradas[indice].nombre,'_');
        if (p_pid == NULL) return -1;
        p_pid++;
        fprintf(stderr,"p_pid %s\n",p_pid);
        pid_t pid = atoi(p_pid);
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
        char prueba[80];

        strcpy(prueba,argv[2]);
        strcat(prueba,buffer_entradas[indice].nombre);
        strcat(prueba,"/prueba.dat");
        
        int indice_escrituras = 0;
        memset(buffer_escrituras,0,sizeof(buffer_escrituras));
        int bLeidos = mi_read(prueba,buffer_escrituras,offset,sizeof(buffer_escrituras));
        while (bLeidos > 0) {
            //struct REGISTRO escritura;
            if (pid == buffer_escrituras[indice_escrituras].pid) {
                if (indice_escrituras == 0) {
                    info.pid = pid;
                    info.nEscrituras = 0;
                    info.PrimeraEscritura = buffer_escrituras[indice_escrituras];
                    info.UltimaEscritura = buffer_escrituras[indice_escrituras];
                    info.MenorPosicion = buffer_escrituras[indice_escrituras];
                    info.MayorPosicion = buffer_escrituras[indice_escrituras];
                } else {
                    if (info.UltimaEscritura.nEscritura < buffer_escrituras[indice_escrituras].nEscritura) {
                        info.UltimaEscritura = buffer_escrituras[indice_escrituras];
                    } else if (info.PrimeraEscritura.nEscritura > buffer_escrituras[indice_escrituras].nEscritura) {
                        info.PrimeraEscritura = buffer_escrituras[indice_escrituras];
                    }

                    if (info.MayorPosicion.nRegistro < buffer_escrituras[indice_escrituras].nRegistro) {
                        info.MayorPosicion = buffer_escrituras[indice_escrituras];
                    } else if (info.MenorPosicion.nRegistro > buffer_escrituras[indice_escrituras].nRegistro) {
                        info.MenorPosicion = buffer_escrituras[indice_escrituras];
                    }
                }
                info.nEscrituras++;
            }
            indice_escrituras++;
            if (indice_escrituras % cant_registros_buffer_escrituras == 0) {
                offset += sizeof(buffer_escrituras);
                indice_escrituras = 0;
                memset(buffer_escrituras,0,sizeof(buffer_escrituras));
                bLeidos = mi_read(prueba,buffer_escrituras,offset,sizeof(buffer_escrituras));
            }
        }
        info.UltimaEscritura = buffer_escrituras[indice_escrituras];
        mi_stat(ruta_informe,&stat); //ctr error
        //fprintf(stderr,"%s %d\n",ruta_informe,stat.tamEnBytesLog);
        mi_write(ruta_informe,&info,stat.tamEnBytesLog,sizeof(struct INFORMACION));
    }

    return bumount();
}
