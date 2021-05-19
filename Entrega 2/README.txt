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

En relación a esto se ha tenido que suprimir el valor de EXIT_FAILURE de la librería otorgada y usar nuestros propios códigos de error
(0 si todo va bien y 1 si ha habido algún error).

Por otra parte, a diferencia de la anterior entrega del proyecto, se han implementado algunas de las mejoras que contempla la plantilla de corrección:
    - comando mi_ls mejorado para mostrar más información sobre cada entrada del directorio
    - implementación del comando mi_touch para la creación de ficheros, dejando mi_mkdir solo para la creación de directorios
    - escritura desde fichero externo en escribir.c
    - mejora del init_MB()
    - implementación de un buffer de entradas en buscar_entrada() y mi_dir() para reducir las llamadas a lecturas
    - creación del comando extra mi_rmdir que elimina los directorios vacíos a excepción del directorio raíz