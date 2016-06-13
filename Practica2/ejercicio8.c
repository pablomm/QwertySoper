/**
* @brief Modulo con main y funciones del ejercicio 8
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio8.c
* @date 2016/03/13
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

/**
* @brief Numero de procesos a generar
*/
#define N_PROCESOS 5

/**
* @brief Iteraciones del bucle de SIGUSR2
*/
#define N_USR2 3

/**
* @brief Funcion encargada de la captura de SIGUSR1
* @param int sig: Numero de sennal
*/
void manejador_SIGUSR1(int sig);

/**
* @brief Funcion encargada de la captura de SIGUSR2
* @param int sig: Numero de sennal
*/
void manejador_SIGUSR2(int sig);

/**
* @brief Funcion encargada de la captura de SIGTERM
* @param int sig: Numero de sennal
*/
void manejador_SIGTERM(int sig);

/**
* El programa programa crea N_PROCESOS en serie
* y realiza una serie intercambio de sennales
* imprimiendo por entrada estandard los pasos
* @brief Main del ejercicio 3a
*/

int main(void){
    int i,k,n;
    pid_t pid[N_PROCESOS];
    
    pid[0] = getpid();

    /* Definimos comportamientos de sennales */
    if(signal(SIGUSR1,manejador_SIGUSR1)==SIG_ERR){
        perror("Error en la captura de SIGUSR1");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGUSR2,manejador_SIGUSR2)==SIG_ERR){
        perror("Error en la captura de SIGUSR2");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGTERM,manejador_SIGTERM)==SIG_ERR){
        perror("Error en la captura de SIGTERM");
        exit(EXIT_FAILURE);
    }

    /* Bucle para crear la serie de procesos */
    for(i=1; i<N_PROCESOS; i++){
        pid[i] = fork();

        if(pid[i] == -1){ /* Caso de error */
            perror("Error en el fork");
            exit(EXIT_FAILURE);

        } else if(pid[i] == 0){ /* Ejecucion del hijo */
            printf("Proceso %d - Proceso padre %d\n",getpid(), getppid());
            continue;

        } else { /* Ejecucion del padre */
            break;
        }   
    }
    /* Protocolo sennal USR1 */
    if(i == N_PROCESOS){ /* Ultimo Hijo */
        sleep(5);
        printf("Proceso %d - Enviando SIGUSR1 a %d\n",getpid(),getppid());
        kill(getppid(),SIGUSR1);

    } else if(i == 1) { /* Proceso raiz */
        pause();
        printf("Proceso %d - Enviando SIGUSR2 a %d\n",getpid(),pid[i]);
        kill(pid[i],SIGUSR2);

    } else { /* Resto de procesos */
        pause();
        printf("Proceso %d - Enviando SIGUSR1 a %d\n",getpid(),getppid());
        kill(getppid(),SIGUSR1);
    }
    /* Guardamos invariante para no recalcularlo */
    n = i % N_PROCESOS;
    /* Protocolo sennal USR2 */
    for(k = (i==1) ? 1 : 0; k< N_USR2; k++){ 
        pause();
        printf("Proceso %d - Enviando SIGUSR2 a %d\n",getpid(),pid[n]);
        kill(pid[n],SIGUSR2);
    }

    /* Protocolo de sennal SIGTERM */
    pause();
    printf("Proceso %d - Enviando SIGTERM a %d\n",getpid(),pid[n]);
    kill(pid[n],SIGTERM);

    if(i==1){ /* El proceso raiz espera su SIGTERM */
        pause();
    }
    
    printf("Proceso %d - Finalizando ejecucion\n",getpid());
    
    /*wait(NULL);*/ /* Descomentar esta linea para no dejar procesos zombies */

    exit(EXIT_SUCCESS);
}

void manejador_SIGUSR1(int sig){
    printf("Proceso %d - Recibida SIGUSR1\n",getpid());
    sleep(2);
}

void manejador_SIGUSR2(int sig){
    printf("Proceso %d - Recibida SIGUSR2\n",getpid());
    sleep(1);
}

void manejador_SIGTERM(int sig){
    printf("Proceso %d - Recibida SIGTERM\n",getpid());
    sleep(1);
}
