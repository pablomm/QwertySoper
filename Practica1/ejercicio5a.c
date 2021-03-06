/**
* @brief Modulo del apartado a) del ejercicio 5
*
* Se genera secuencialmente una serie de procesos
* que imprimen su pid y el de su padre
* @file ejercicio5a.c
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 3

/**
* @brief main del programa del ejercicio 5a
* @return EXIT_SUCCESS si fue todo bien o EXIT_FAILURE en caso contrario
*/
int main (void) {
    int pid;
    int i;

    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);

        } else if (pid ==0) {
            printf ("PADRE %d\n", getppid());
            printf("HIJO %d\n", getpid());

        }else{
            /* Se hace un wait para esperar  al proceso hijo */
            wait(NULL);
            break;
        }
    }
    exit(EXIT_SUCCESS);
}
