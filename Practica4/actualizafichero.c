/**
 * Funcion para actualizar el registro del examen
 * 
 * 
 * @brief Funcion para ejercicio 2
 * @author Pablo Marcos Manchon
 * @author David Nevado Catalan
 * @file actualizafichero.c
 * @date 32/04/2016
 */
#define _XOPEN_SOURCE 700


#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h> 
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "actualizafichero.h"

int  end_flag = 0;
/**
 * @brief Manejador de SIGALRM
 * @param sig Sennal recivida
 */
void updatehandler_SIGALRM(int sig){
    printf("File updated\n");
    end_flag=1;

}


/**
 *
 *Funcion principal, se encarga de ejecutar el comando 
 * ps -ef | grep pid | grep -v grep y guarda el resultado
 * en un fichero
 * @brief Actualiza un fichero con la carga el sistema  
 */
int actualiza_fichero(){

    /*Variables*/
    int fd[3][2];
    int cpid[3];
 
    char spid[10];
    char filename[30];

    int maxbuf=500000;
    

    char ch;

    int count;

    FILE * pf;
/********************/
    
    if(signal(SIGALRM, updatehandler_SIGALRM)==SIG_ERR){
        perror("No se pudo armar el manejador");
        exit(EXIT_FAILURE);

    }
    
    sprintf(spid,"%d", getpid());
    

    sprintf(filename, "SIGHUP_%d_lista_proc.txt", getpid());
    printf("Updating %s...\n", filename);

    pf=fopen(filename,"a+"); 

    if(pipe(fd[0])==-1){
        perror("Error en la tuberia");
        exit(EXIT_FAILURE);
    }
    cpid[0]=fork();    
    switch(cpid[0]){
        case -1:
            perror("Error en fork");
            exit(EXIT_FAILURE);
        case 0:
            if(pipe(fd[1])==-1){
                perror("Error en la tuberia");
                exit(EXIT_FAILURE);
            }
            cpid[1]=fork();
            switch(cpid[1]){
                case -1:
                    perror("Error en fork");
                    exit(EXIT_FAILURE);
                case 0:
                    if(pipe(fd[2])==-1){
                        perror("Error en la tuberia");
                        exit(EXIT_FAILURE);
                    }
                    cpid[2]=fork();
                    switch(cpid[2]){
                        case -1:
                            perror("Error en fork");
                            exit(EXIT_FAILURE);
                        case 0:
                            close(fd[2][0]);
                            close(1);
                            dup(fd[2][1]);
                            execlp("ps","ps","-ef",(char *)0);
                            perror("Error al lanzar comando ps");
                            exit(EXIT_FAILURE);
                        default:
                            close(fd[2][1]);
                            close(0);
                            dup(fd[2][0]);

                            close(fd[1][0]);
                            close(1);
                            dup(fd[1][1]);
                            //sleep(1);
                            wait(NULL);
                            execlp("grep","grep", spid, (char *)0);
                            perror("Error al lanzar comando grep (1)");
                            exit(EXIT_FAILURE);
                    }
                default:
                    close(fd[1][1]);
                    close(0);
                    dup(fd[1][0]);

                    close(fd[0][0]);
                    close(1);
                    dup(fd[0][1]);
                    //sleep(1);
                    wait(NULL);
                    execlp("grep","grep", "-v","grep", (char *)0);
                    perror("Error al lanzar comando grep (1)");
                    exit(EXIT_FAILURE);
            }
        default:
            fprintf(pf, "\nNew Update.\n--------------------------------------\n\n");
            wait(NULL);
            alarm(2);
            for (count=0;read(fd[0][0], &ch,sizeof(ch))==1 && count<maxbuf ; count++){
                if(end_flag){
                    fprintf(pf,"\nWritting time expired\n");
                    break;
                }
                
                fprintf(pf,"%c",ch);
                
                
            }
            
    }
    end_flag=0;
    fclose(pf);
    
    return 0;

}
