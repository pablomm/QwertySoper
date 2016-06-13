/**
* @brief Modulo con main y funciones del ejercicio 10
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio10.c
* @date 2016/03/16
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>

/**
* @brief Tamanno maximo de las palabras de la frase
*/
#define MAX 10
/**
* @brief Numero maximo de lecturas que realiza el padre
*/
#define N_ITER 50
/**
* @brief Tiempo de espera en segundos entre lecturas (y entre escrituras)
*/
#define T_ESPERA 5

/**
* @brief Funcion encargada de la captura de SIGUSR1
* @param int sig: Numero de sennal
*/
void manejador_SIGUSR1(int signal);
/**
* @brief Funcion encargada de la captura de SIGALRM
* @param int sig: Numero de sennal
*/
void manejador_SIGALRM(int signal);

/**
* El programa genera un hijo. El hijo empieza a escribir
* palabras de la frase en un fichero cada T_ESPERA segundos,
* cuando escribe FIN termina.
* El padre lee del fichero, y cuando lee FIN, relanza el hijo.
* El programa termina despues de N_ITER lecturas.
* @brief Main del ejercicio 10
*/

int main(void){
    pid_t childpid = -1;
    FILE * pfile = NULL;
    int i=0;
    int flag=0;
    sigset_t mask;
	const char fichero[]= "fichero.txt";
    char cadena[][MAX] = {"EL", "PROCESO", "ESCRIBE", "EN", "UN", "FICHERO",
"HASTA", "QUE", "LEE", "LA", "CADENA", "FIN"};
    char palabra[MAX];
    char buffer[MAX];


  /*Mascara para esperar a la alarma y a la sennal SIGUSR1*/
    if(sigfillset(&mask)==-1){
        perror("Error en la mascara");
        exit(EXIT_FAILURE);
    }
    if(sigdelset(&mask,SIGALRM)==-1){
        perror("Error en la mascara");
        exit(EXIT_FAILURE);
    }        
    if(sigdelset(&mask,SIGUSR1)==-1){
        perror("Error en la mascara");
        exit(EXIT_FAILURE);
    }        
 
    /*Armamos un manejador para la alarma*/
    if(signal(SIGALRM, manejador_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }
    /*Armamos el manejador de SIGUSR1*/
    if(signal(SIGUSR1, manejador_SIGUSR1)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }

    /*Bucle principal del programa*/
    for (i=0;i<N_ITER;){    
        
        childpid=fork();
	    if(childpid==-1){
            perror("Error en la creacion del porceso hijo");
            exit(EXIT_FAILURE);
        }

	    /*Linea de ejecucion del padre*/
        if(childpid>0){
            /*Esperamos hasta que se reciba la sennal del hijo para abrir el fichero */
            sigsuspend(&mask);

            flag=0;
            pfile=fopen(fichero, "r");
               if(pfile==NULL){
                   perror("No se pudo abrir el fichero");
                   exit(EXIT_FAILURE);
                }
            for(;i<N_ITER && flag == 0;i++){
               
                fgets(buffer, MAX, pfile);
                printf("He leido la palabra %s", buffer);
                strtok(buffer, "\n");
                if(strcmp("FIN", buffer)==0){
                    fclose(pfile);
                    if(wait(NULL)==EXIT_FAILURE){
                        exit(EXIT_FAILURE);
                    }
                    flag=1;
                } else {
                    /*Se genera una alarma cada 5 segundos*/
                    if(alarm(T_ESPERA)){
                        perror("Ya habia una alarma creada");
                        exit(EXIT_FAILURE);
                    }
                    /*Esperamos hasta que se reciba la alarma */
                    sigsuspend(&mask);
                   
                }
         
            }
		
        /*Proceso hijo*/
	    }
        if(childpid==0){
            /*Lanzamiento del proceso hijo*/
            printf("Lanzamiento de un hijo con pid: %d\n", getpid());

            /*Seed*/
		    srand(getpid() + time(NULL));
            
            pfile=fopen(fichero, "w+");
            if(pfile==NULL){
                perror("No se pudo abrir el fichero");
                exit(EXIT_FAILURE);
            } 
            /*Bucle principal*/
            while(1){
                   
                
                strcpy(palabra,cadena[rand()%12]);
                fprintf(pfile,"%s\n", palabra);
                printf("Soy hijo, y he escrito la palabra %s\n", palabra);

                
                if(strcmp("FIN", palabra)==0){
                    fclose(pfile);
                    kill(getppid(),SIGUSR1);
                    printf("Soy el hijo, y me acabo\n");
                    exit(EXIT_SUCCESS);
                    
                }

            } 
	    }

    }
    if(flag==0){
        kill(childpid, SIGKILL);
        wait(NULL);
    }
    printf("Se han hecho %d lecturas\n",i);
    exit(EXIT_SUCCESS);

}

void manejador_SIGALRM(sig){
    if(signal(SIGALRM, manejador_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }
}
void manejador_SIGUSR1(sig){
    
    if(signal(SIGUSR1, manejador_SIGUSR1)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }
}
