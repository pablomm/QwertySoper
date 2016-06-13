/**
* @brief Modulo con main del ejercicio 3a
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio3a.c
* @date 2016/03/04
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define N_HIJOS 100

/**
* El programa lanza 100 procesos hijos, el cual cada uno
* imprime un numero aleatorio, despues el padre espera a
* todos, se calcula el tiempo que tarda en suceder
*
* @brief Main del ejercicio 3a
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
*/

int main(void) {
	int i;
    clock_t time;

    /* Guardamos el tiempo inicial */
    time = clock();

	for (i=0; i< N_HIJOS; i++){
		switch(fork()){
            case -1: /* Caso de error */
                perror("Error en el fork");
                exit(EXIT_FAILURE);

            case 0: /* Ejecucion del hijo */
                /* Actualizamos la semilla */
                srand(getpid()); 
                fprintf(stdout,"Numero aleatorio %d: %d\n",i+1,rand());
                fflush(stdout);
                exit(EXIT_SUCCESS);
            default: /* Ejecucion del padre */
                wait(NULL); /* Espera al hijo */
	    }
    }
    /* Calculamos la diferencia de tiempo */
    time = clock() - time; 
    fprintf(stdout,"Tiempo transcurrido: %.3f ms\n",(time*1000.0)/CLOCKS_PER_SEC);
	exit(EXIT_SUCCESS);
}
