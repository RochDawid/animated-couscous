/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "simulacion.h"
 
struct INFORMACION {
  int pid;
  unsigned int nEscrituras; //validadas 
  struct REGISTRO PrimeraEscritura;
  struct REGISTRO UltimaEscritura;
  struct REGISTRO MenorPosicion;
  struct REGISTRO MayorPosicion;
};

int saveInformacion(struct INFORMACION info, char *output);