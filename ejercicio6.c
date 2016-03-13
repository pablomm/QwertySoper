/**
* @brief Modulo con main del ejercicio 6
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio6.c
* @date 2016/03/04
*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
* @brief Tiempo de espera del padre
*/
#define T_PADRE 30

/**
* @brief Tiempo de espera del hijo
*/
#define T_HIJO 5

/**
* El programa lanza un proceso hijo que imprime su pid 
* cada 5 segundos, a los 30 segundos el padre manda una
* señal para finalizar la ejecucion del proceso hijo y
* finaliza la ejecucion del programa
* @brief Main del ejercicio 6
*/
int main(void){
    pid_t pid;

    switch(pid=fork()){
        case -1:
            perror("Error en el fork()");
            exit(EXIT_FAILURE);

        case 0: /* Ejecucion proceso hijo */             
            while(1){
                fprintf(stdout,"Soy el proceso hijo con PID: %d\n",getpid());
                fflush(stdout);       
                sleep(T_HIJO);
            }
            exit(EXIT_FAILURE);

        default: /* Ejecucion del padre */
            sleep(T_PADRE);
            kill(pid,SIGKILL);
            wait(NULL); /* Limpiamos al hijo de la tabla de procesos */
    }
    exit(EXIT_SUCCESS);
}
