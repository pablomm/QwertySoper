/**
* @brief Modulo del apartado a) del ejercicio 4 
*
* Contiene un programa que lanza varios hijos y en cada fork()
* el proceso hijo imprime en la salida estandar su pid y el de
* su padre
* @file ejercicio4a.c
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/

#include <stdio.h>
#include <stdlib.h>

#define NUM_PROC 3

/**
* @brief main del programa del ejercicio 4a
* @return EXIT_SUCCESS si fue todo bien o EXIT_FAILURE en caso contrario
*/

int main (void){
    int pid;
    int i;

    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        
        } else if (pid ==0) { /* Rama de ejecucion del hijo */
            printf("HIJO  %d\n",getpid());
            printf ("PADRE %d \n",getppid());
        }
    }
    exit(EXIT_SUCCESS);
}
