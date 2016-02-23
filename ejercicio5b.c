/**
* @brief Modulo del apartado b) del ejercicio 5 
*
* Contiene un programa que lanza varios hijos  
* esperando hasta que termine la ejecucion de todos ellos
* Cada hijo imprime su pid y el de su padre
* @file ejercicio5b.c
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/

#include <stdio.h>
#include <stdlib.h>

#define NUM_PROC 3

int main (void){
    int pid;
    int i;

    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);

        } else if (pid ==0) { /* Rama de ejecucion del hijo */
            printf ("PADRE %d\n", getppid());
            printf("HIJO %d\n", getpid());
            break;

        }else{ /* Rama de ejecucion del padre */
            wait();
        }
    }
    exit(EXIT_SUCCESS);
}
