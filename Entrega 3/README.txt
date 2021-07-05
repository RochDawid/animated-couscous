* * * * * * * * * * * * * * *
* Integrantes del grupo DST *
*			                *
*   Sergi Moreno Pérez      *
*   Antoni Payeras Munar    *
*   Dawid Michal Roch Móll  *
* * * * * * * * * * * * * * *

Esta práctica consiste en implementar un sistema de ficheros (basado en el ext2 de Unix) en un disco virtual (fichero), el módulo de gestión de 
ficheros del sistema operativo y la interacción de procesos de usuarios.
Para ello se ha dividido en 3 entregas. Esta entrega corresponderá a la tercera parte del proyecto.

Durante la implementación de esta parte del proyecto hemos tenido algunos problemas para entender en que consistía y como implementar los semáforos
y como hacer para conseguir la granularidad. Tras observarlo detenidamente y dedicarle tiempo, conseguimos implementarlo entre todos
los componentes del equipo.

Por otra parte, se han implementado algunas de las mejoras que contempla la plantilla de corrección:
    - mejora del init_MB()
    - implementación de un buffer de entradas en buscar_entrada() y mi_dir() para reducir las llamadas a lecturas
    - implementación del comando mi_touch para la creación de ficheros, dejando mi_mkdir solo para la creación de directorios
    - comando mi_ls mejorado para mostrar más información sobre cada entrada del directorio y colores para diferenciar entre directorios y ficheros
    - creación del comando extra mi_rmdir que elimina los directorios a excepción del directorio raíz
    - control de las secciones críticas de la capa de ficheros (CORREGIDO)
