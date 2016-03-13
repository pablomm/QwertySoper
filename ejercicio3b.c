/**
* @brief Modulo con main del ejercicio 3b
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio3b.c
* @date 2016/03/04
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

/**
* @brief Numero de hijos que seran lanzados
*/
#define N_HIJOS 100

/**
* Funcion llamada por los hilos, calcula e imprime
* por la salida estandard un numero aleatorio
*
* @brief Imprime un numero aleatorio
* @param int *i: Numero de hilo 
*/
void printAleatNum(int *i){
    fprintf(stdout,"Numero aleatorio %d: %d\n",*(i)+1,rand());
    fflush(stdout);
    pthread_exit(NULL);
}

/**
* El programa lanza 100 hilos, donde cada uno
* imprime un numero aleatorio, mientras
* se calcula el tiempo que transcurre
*
* @brief Main del ejercicio 3b
*/
int main(void) {
	int i;
    clock_t time;
    pthread_t hilo[N_HIJOS];

    /* Guardamos el tiempo inicial */
    time = clock();

	for(i=0; i<N_HIJOS; i++){ /* Bucle para crear los hilos */
        pthread_create(&(hilo[i]),NULL,(void * (*)(void *))printAleatNum,&i);
        pthread_join(hilo[i],NULL);
    }

    /* Calculamos la diferencia de tiempo */
    time = clock() - time; 
    fprintf(stdout,"Tiempo transcurrido: %.3f ms\n",(time*1000.0)/CLOCKS_PER_SEC);
	exit(EXIT_SUCCESS);
}
