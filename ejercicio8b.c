/**
* @brief Modulo del apartado b) del ejercicio 8
*
* Programa que solicita al usuario un comando para ejecutar y despues ofrece la
* posibilidad de ejecutarlo en primer o segundo plano
*
*
* @file ejercicio8b.c
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define TAM 40


/**
* @brief main del programa del ejercicio 8b
* @return Devuelve EXIT_FAILURE en caso de fallo, y EXIT_SUCCESS si se ejecuta
* el comando en segundo plano y termina con exito.
* Los parametros junto al comando que se introduzcan son ignorados.
*/
int main(void){
    char comando[TAM];
    int plano=0; /*Controlara el bucle que asegura una entrada correcta*/
    int pid=-1;

    /*Entrada del comando*/
    printf("Introduzca el comando: ");
    fgets(comando,TAM,stdin);

	/* Sustituimos el \n y los espacios almacenados por un \0 */
	strtok(comando," \n");

    /*Eleccion de ejecucion: primer o segundo plano, se ejecuta en bucle hasta
    que se  elige una opcion valida*/
    while(plano !=1 && plano != 2){
        printf("Desea ejecutar el comando en primer plano (1) o en segundo plano(2)?");
        scanf("%d",&plano);

        if(plano==1){ /* Caso Foreground */
            execlp(comando,comando, NULL);

        } else if(plano ==2){ /* Caso Background */
			if((pid=fork()) < 0){ /* Error en el fork() */
				perror("Error al realizar el fork()");
				exit(EXIT_FAILURE);

			} else if (pid==0){ /* Rama de ejecucion del hijo */
                setsid();
				execlp(comando,comando, NULL);
                

			} else { /* Rama de ejecucion del padre */
				/* wait(NULL); */
				exit(EXIT_SUCCESS);
			}

        } else {
        printf("No ha introducido una opcion correcta. Por favor, introduzca 1 "
              "para ejecutar el comando en primero plano, o 2 para ejecutarlo"
              "en segundo plano\n");
        }
    }

    /*Si hay un error en execl*/
    perror("No pudo ejecutarse el comando");
    exit(EXIT_FAILURE);

}
