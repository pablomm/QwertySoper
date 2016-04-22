/**
 * Ejercicio 3 Practica 4 SOPER
 * Ejercicio basado en el envio de mensajes
 * entre procesos
 * @brief Ejercicio 3 Practica 4
 * @author Pablo Marcos Manchon
 * @author David Nevado Catalan
 * @file cadena_montaje.c
 * @date 22/04/2016
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/**
 * @brief Maxima longitud a leer
 */
#define MAX_LENGTH 4096

/**
 * @brief Clave comun para generar clave
 */
#define N 33

/**
 * @brief Numero de procesos lanzados
 */
#define N_PROCESOS 3

/**
 * @brief Identificador del proceso A
 */
#define A 0

/**
 * @brief Identificador del proceso B
 */
#define B 1

/**
 * @brief Identificador del proceso C
 */
#define C 2

/**
 * @brief Estructura para mandar mensajes
 */
typedef struct _Mensaje{
    /**Id del mensaje */
    long id; 
    /** Tipo de mensaje */
    int valor;
    /** Texto del mensaje */
    char aviso[MAX_LENGTH];
} mensaje;

/**
 * Funcion que transforma una String a mayusculas
 * @brief Funcion para convertir a mayusculas
 * @param toUpper string a ser convertida
 */
void upper(char* toUpper){
    int i;

    /* Control de errores */
    if(toUpper[0]=='\0'){
        return;
    }
    for(i=0; toUpper[i] != '\0';i++){
        toUpper[i] = toupper(toUpper[i]);
    }
}

/**
 * El proceso crea una cola de mensajes y lanza otros
 * dos procesos, con los cuales intercambiara mensajes
 * escribiendo y leyendo de archivos.
 * Su ejecucion será de la siguiente forma 
 * ./Cadena_montaje <entrada> <salida>
 * @brief Main del ejercico cadena de montaje.
 * @param argv Nombre de archivos de entrada y salida
 *
 */
int main(int argc,char *argv[]){
    key_t clave;
    int msqid,i,flag=0;
    mensaje msg;
    FILE *fp = NULL;
    int id,pid[N_PROCESOS];
    char buffer[MAX_LENGTH]="\0"; 


    /* Comprobacion de parametros */
    if(argc != 3){
        perror("Numero de parametros incorrecto");
        exit(EXIT_FAILURE);
    }

    /* Obtenemos clave para generar la cola de mensajes */
    clave = ftok ("/bin/ls", N);
    if (clave == (key_t) -1){
        perror("Error al obtener clave para cola mensajes\n");
        exit(EXIT_FAILURE);
    }
    
    /* Creamos la cola de mensajes */
    msqid = msgget (clave, 0666 | IPC_CREAT);
    if (msqid == -1){
        perror("Error al obtener identificador la para cola mensajes");
        exit(EXIT_FAILURE);
    }

    printf("Cola de mensajes creada con msqid: %d\n",msqid);
    printf("Proceso A - PID: %d\n",pid[0] = getpid());

    /* Lanzamos procesos B y C */
    for(id=1; id != 0; id = (id +1) % N_PROCESOS){
        /* Caso de error */        
        if((pid[id] = fork()) == -1){
            perror("Error en el fork");
            exit(EXIT_FAILURE);
        
        /* Caso del hijo */        
        } else if (pid[id] == 0) {
            printf("Proceso %c - PID: %d\n",(id == 1) ? 'B' : 'C',getpid());
            break; /* Salimos del bucle */
        }        
    }

    switch(id){
        /* Proceso A */
        case A:
            /* Abrimos el archivo */
            fp = fopen(argv[1],"r");
            if(!fp){ /* Error abriendo archivo */
                /* Matamos y recogemos hijos */
                for(i=1; i < N_PROCESOS; i++){
                    kill(pid[i],SIGKILL);
                    wait(NULL);
                }
                perror("Error abriendo archivo");
                /* Eliminamos la  cola de mensajes */
                msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
                exit(EXIT_FAILURE);
            }

            printf("Proceso A - Abierto archivo '%s'\n\n",argv[1]);

            /* Leemos el archivo en bloques de hasta 4Kb */
            while(fread(buffer,sizeof(char),MAX_LENGTH,fp) != 0){
                
                if(flag == 0){ /* Caso primera lectura */
                    flag = 1;
                } else { /* Resto de lecturas */
                   /* Esperamos a que haya acabo de procesar el texto */
                   msgrcv (msqid, (struct msgbuf *)&msg, sizeof(mensaje) - sizeof(long), 3, 0);
                   /* Protocolo de finalizacion de error */
                   if(msg.valor == 1){ 
                        msg.id = 1; /* Enviamos mensaje a proceso B para que finalice */
                        msg.valor = 1;
                        msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
                        /* Cerramos archivo, borramos cola de mensajes y recogemos hijos */
                        fclose(fp);
                        msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
                        for(i=1; i < N_PROCESOS;i++){
                            wait(NULL);
                        }
                        exit(EXIT_FAILURE);
                    }
                }

                /* Enviamos mensaje con linea leida */
                msg.id = 1;
                msg.valor = 0;
                strcpy (msg.aviso, buffer);
                printf("Proceso A - Enviando mensaje con texto: \n%s\n",msg.aviso);
                /* Enviamos mensaje con id 1 */
                msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
            }
            printf("Proceso A - Terminado de leer archivo\n");
            fclose(fp); /* Cerramos el archivo */
            printf("Proceso A - Cerrado archivo '%s'\n",argv[1]);
            
            /* Mandamos mensaje de finalizacion a B */
            msg.id = 1;
            msg.valor = 1; /* Codigo de finalizacion */
            msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);

            /* Esperamos mensaje de finalizacion de C */
            msgrcv (msqid, (struct msgbuf *)&msg, sizeof(mensaje) - sizeof(long), 3, 0);
                        
            /* Esperamos a los hijos */
            for(i=1; i < N_PROCESOS;i++){
                wait(NULL);
            }
            /* Borramos el semaforo */
            msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
        break;

        case B:

            while(1){
                msgrcv (msqid, (struct msgbuf *)&msg, sizeof(mensaje) - sizeof(long), 1, 0);
                /* Caso mensaje de finalizacion */
                if(msg.valor == 1){
                    msg.id = 2;
                    /* Enviamos mensaje de finalizacion */
                    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
                    exit(EXIT_SUCCESS);
                }

                printf("Proceso B - Recibido mensaje\n");
                msg.id = 2;
                /* Convertimos a mayusculas el texto */ 
                upper(msg.aviso); 
                printf("Proceso B - Enviando mensaje con texto: \n%s\n",msg.aviso);
                /* Enviamos mensaje con id 2 */
                msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
            }
        break;
            
        case C:
            while(1){
                 /* Caso primer mensaje escrito */
                 if(flag==0){
                    /* Abrimos archivo */
                    fp = fopen(argv[2],"w");
                    if(!fp){
                        fprintf(stderr,"Proceso C - Error escribiendo en archivo '%s'\n",argv[2]);
                        /* Envia mensaje de error a proceso A que inicia protocolo de error */
                        msg.valor = 1;  
                        msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
                        exit(EXIT_FAILURE);
                    }

                    printf("Proceso C - Abierto archivo '%s'\n\n",argv[2]); 
                    flag = 1;
                }

                 /* Esperamos a recibir un mensaje */
                 msgrcv(msqid, (struct msgbuf *)&msg, sizeof(mensaje) - sizeof(long), 2, 0);
                 msg.id = 3;
                 /* Caso mensaje de finalizacion */
                 if(msg.valor == 1){
                    fclose(fp);
                    printf("Proceso C - Cerrado archivo '%s'\n",argv[2]);
                    /* Enviamos mensaje de finalizacion a A */
                    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
                    exit(EXIT_SUCCESS);
                }

            printf("Proceso C - Recibido mensaje\n");
            /* Escribimos en el archivo */
            fprintf(fp,"%s",msg.aviso);
            printf("Proceso C - Mensaje escrito en el fichero de salida\n");   
            /* Enviamos a A mensaje de confirmacion */         
            msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 0);
        }   
        break;

    }
    exit(EXIT_SUCCESS);
}
