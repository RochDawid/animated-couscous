* * * * * * * * * * * * * * *
* Integrantes del grupo DST *
*			                *
*   Sergi Moreno Pérez      *
*   Antoni Payeras Munar    *
*   Dawid Michal Roch Móll  *
* * * * * * * * * * * * * * *

Esta práctica consiste en implementar un sistema de ficheros (basado en el ext2 de Unix) en un disco virtual (fichero), el módulo de gestión de 
ficheros del sistema operativo y la interacción de procesos de usuarios.
Para ello se ha dividido en 3 entregas. Esta entrega corresponderá a la segunda parte del proyecto.

Durante la implementación de esta parte del proyecto hemos tenido algunos problemas con los valores que retornaban algunas funciones que posteriormente
eran recogidos desde otras llamadas a funciones y se trataban de forma incorrecta. Para solucionar esta situación hemos verificado que todas las
funciones retornaban los valores pertinentes para que las llamadas a estas no creasen estos problemas.

En relación a esto se ha tenido que modificar el valor de EXIT_FAILURE de la librería otorgada.

Por otra parte, a diferencia de la anterior entrega del proyecto, se han implementado algunas de las mejoras que contempla la plantilla de corrección
como el mi_ls mejorado, la implementación del comando mi_touch, escritura desde fichero externo en escribir.c, la mejora del init_MB, la implementación
de un buffer de entradas en buscar_entrada() y mi_dir() y se ha creado un comando extra mi_rmdir que elimina los directorios vacíos.