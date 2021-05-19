* * * * * * * * * * * * * * *
* Integrantes del grupo DST *
*			                *
*   Sergi Moreno Pérez      *
*   Antoni Payeras Munar    *
*   Dawid Michal Roch Móll  *
* * * * * * * * * * * * * * *

Esta práctica consiste en implementar un sistema de ficheros (basado en el ext2 de Unix) en un disco virtual (fichero), el módulo de gestión de 
ficheros del sistema operativo y la interacción de procesos de usuarios.
Para ello se ha dividido en 3 entregas. Esta entrega corresponderá a la primera parte del proyecto.

Para llevar a cabo el control de errores de la práctica nos surgieron un par de dudas sobre el nivel de abstracción de estos, de manera que hemos
comprobado el valor que retornan todas las funciones que pueden producir un error con la finalidad de poder realizar una mejor gestión de los
posibles fallos durante el tiempo de ejecución.
A pesar de todo, somos conscientes de que debido a estas dudas quizás el control de errores no haya sido el idóneo pero es suficientemente adecuado 
para este punto de la práctica, con tiempo para realizar cambios y rectificar aquellos errores que se hayan podido producir.

Un poco relacionado con el apartado anterior, cabe decir que de momento no hemos implementado mejoras en el código porque estamos asimilando los conceptos 
teóricos y, como hemos mencionado, hay algunos conceptos que aún nos generan dudas. Una vez resolvamos esto, muy probablemente las acabaremos implementando.