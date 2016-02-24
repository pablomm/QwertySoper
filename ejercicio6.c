/**
* @brief Ejercicio 6
*
* Programa que comprueba que unos procesos padre e hijo no comparten variables
*
*
* @file ejercicio6.c
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/


#include <stdlib.h>
#include <stdio.h>

#define TAM 30

/**
* @brief main del programa del ejercicio 6
* @return EXIT_SUCCESS si fue todo bien o EXIT_FAILURE en caso contrario
*/

int main(void){
    int status;
    char * buf =NULL;
    pid_t f=-1;
	/*Reserva de memoria para 20 caracteres*/
    buf=malloc(TAM* sizeof(char));
    /*Control de la reserva de memoria*/	
    if(buf==NULL){
        perror("Error en la reserva de memoria\n");
        exit(EXIT_FAILURE);
    }
    /*Indicamos que buf es un string vacio*/
    buf[0]=0;
    /*Lanzamiento de un proceso hijo*/
    f=fork();

    if(f==-1){
        perror("Error al lanzar hijo\n");
        exit(EXIT_FAILURE);
    }
	/*Linea de ejecucion del proceso hijo*/
    else if(f==0) {
        /*Solcitud de entrada al usuario */
        fprintf(stdout, "\nPADRE: Introduzca un mensaje corto: ");
        fgets(buf,TAM, stdin);
        free(buf);	
    }

	/*Linea de ejecucion del proceso padre*/
    else {
        /*Salida por pantalla del contenido de buf*/
        /*Si el string esta vacio, mostramos un mensaje informando*/
        if(buf[0]==0){
            printf("HIJO: No hay ningun mensaje.\n");
        }else {
            printf("HIJO: Mensaje que tiene el padre: %s\n", buf);
        }
        free(buf);
        wait(&status);
    }
    exit(EXIT_SUCCESS);
}
