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
    // comprobación de que el número de entradas correspondientes al directorio pasado por parámetro
    // coinciden con NUMPROCESOS
    if (numentradas != NUMPROCESOS) return -1;
    char ruta_informe[80];
    strcpy(ruta_informe,argv[2]);
    strcat(ruta_informe,"informe.txt");
    // creación fichero informe.txt en el mencionado directorio
    mi_creat(ruta_informe,7);
    
    int cant_registros_buffer_escrituras = 256;
    struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
    char output[1000]; // se ha utilizado un tamaño lo suficientemente grande para poder almacenar todos los caracteres

    struct entrada buffer_directorios[NUMPROCESOS];
    int offset = 0;
    // lectura de las entradas directorio del directorio pasado por comando
    memset(buffer_directorios,0,sizeof(buffer_directorios));
    mi_read(argv[2],buffer_directorios,offset,sizeof(buffer_directorios));
    for (int indice = 0;indice < NUMPROCESOS;indice++) {
        struct INFORMACION info;
        // obtención del pid correspondiente al directorio iterado
        char *p_pid = strchr(buffer_directorios[indice].nombre,'_');
        if (p_pid == NULL) return -1;
        p_pid++;
        pid_t pid = atoi(p_pid);
        info.pid = pid;
        char prueba[80];


        strcpy(prueba,argv[2]);
        strcat(prueba,buffer_directorios[indice].nombre);
        strcat(prueba,"/prueba.dat");
        
        offset = 0;
        info.nEscrituras = 0;
        int indice_escrituras = 0;
        // lectura de todos los struct REGISTRO escritos en el fichero
        memset(buffer_escrituras,0,sizeof(buffer_escrituras));
        int bLeidos = mi_read(prueba,buffer_escrituras,offset,sizeof(buffer_escrituras));
        while (bLeidos > 0) {
            if (pid == buffer_escrituras[indice_escrituras].pid) {
                // primera escritura validada e inicialización de los campos del struct
                if (info.nEscrituras == 0) {
                    info.PrimeraEscritura = buffer_escrituras[indice_escrituras];
                    info.UltimaEscritura = buffer_escrituras[indice_escrituras];
                    info.MenorPosicion = buffer_escrituras[indice_escrituras];
                    info.MayorPosicion = buffer_escrituras[indice_escrituras];
                } else {
                    // comparación de las escrituras y actualización en caso de ser necesario
                    if (info.MayorPosicion.nRegistro < buffer_escrituras[indice_escrituras].nRegistro) {
                        info.MayorPosicion = buffer_escrituras[indice_escrituras];
                    } else if (info.MenorPosicion.nRegistro > buffer_escrituras[indice_escrituras].nRegistro) {
                        info.MenorPosicion = buffer_escrituras[indice_escrituras];
                    }
                    if (info.PrimeraEscritura.nEscritura > buffer_escrituras[indice_escrituras].nEscritura) {
                        info.PrimeraEscritura = buffer_escrituras[indice_escrituras];
                    }

                    if (info.UltimaEscritura.nEscritura < buffer_escrituras[indice_escrituras].nEscritura) {
                        info.UltimaEscritura = buffer_escrituras[indice_escrituras];
                    }
                }
                info.nEscrituras++;
            }
            indice_escrituras++;
            // en caso de que se hayan iterado todos los REGISTROS del pruebas.dat iterado, volvemos a cargar el buffer
            if (indice_escrituras % cant_registros_buffer_escrituras == 0) {
                offset += sizeof(buffer_escrituras);
                indice_escrituras = 0;
                memset(buffer_escrituras,0,sizeof(buffer_escrituras));
                bLeidos = mi_read(prueba,buffer_escrituras,offset,sizeof(buffer_escrituras));
            }
        }
        // escritura del struct INFORMACION generado al final del fichero informacion.txt
        memset(output, 0, sizeof(output));
        saveInformacion(info, output);
        fprintf(stderr,"%d) %d escrituras validadas en %s\n",indice+1,info.nEscrituras,prueba);
        if (mi_stat(ruta_informe,&stat) == -1) return -1;
        if (mi_write(ruta_informe,output,stat.tamEnBytesLog,sizeof(output)) == -1) return -1;
    }

    return bumount();
}

/*
    saveInformacion: del struct INFORMACION pasado por parámetro, devuelve un buffer de caracteres con toda su información
                    correctamente formateada
*/
int saveInformacion(struct INFORMACION info, char *output) {
    struct tm *ts;
    char buffer[1000]; // se ha utilizado un tamaño lo suficientemente grande para poder almacenar todos los caracteres
    char time[80];

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "\nPID: %d\n", info.pid);
    strcat(output, buffer);
    sprintf(buffer, "Número de escrituras: %d\n", info.nEscrituras);
    strcat(output, buffer);
    ts = localtime(&info.PrimeraEscritura.fecha);
    strftime(time, sizeof(time), "%a %Y-%m-%d %H:%M:%S", ts);
    sprintf(buffer, "Primera escritura: %d\t%d\t%s\n", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, time);
    strcat(output, buffer);
    ts = localtime(&info.UltimaEscritura.fecha);
    strftime(time, sizeof(time), "%a %Y-%m-%d %H:%M:%S", ts);
    sprintf(buffer, "Última escritura: %d\t%d\t%s\n", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, time);
    strcat(output, buffer);
    ts = localtime(&info.MenorPosicion.fecha);
    strftime(time, sizeof(time), "%a %Y-%m-%d %H:%M:%S", ts);
    sprintf(buffer, "Menor posición: %d\t%d\t%s\n", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, time);
    strcat(output, buffer);
    ts = localtime(&info.MayorPosicion.fecha);
    strftime(time, sizeof(time), "%a %Y-%m-%d %H:%M:%S", ts);
    sprintf(buffer, "Mayor posición: %d\t%d\t%s\n\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, time);
    strcat(output, buffer);

    return 0;
}
