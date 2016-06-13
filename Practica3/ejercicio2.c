
/**
 * Ejercicio 2 de la practica 3
 * Sistemas Operativos 
 * @brief Ejercicio 2
 * @file ejercicio6.c
 * @author Pablo Marcos Manchon pablo.marcosm@estudiante.uam.es
 * @author David Nevado Catalan david.nevadoc@estudiante.uam.es
 * @date 11/04/2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>

#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h> 

#define FILEKEY "/bin/cat"
#define KEY 1300
#define MAXSLEEP 10
#define MAXNAME 80


typedef struct _info{
    char nombre[MAXNAME];
    int id;
} info ;
/**
 * @brief Manejador de la sennal SIGINT
 * @param sennal recibida
 */
void manejador_SIGINT(int signal);
/**
 * @brief Manejador de la sennal SIGUSR1
 * @param sennal recibida
 */
void manejador_SIGUSR1(int signal);
/**
 * Main del ejercico 2, simula un escenario donde se producen problemas por
 * condicion de carrera
 * @brief Main del ejercicio 2
 * @file ejercicio2.c
 * @date 11/04/2016
 */
int main(int argc, char * argv[]){

    int key =-1;
    int i=0;
    int n=0;
    int id_zone=-1;
    int ret =1;
    pid_t childpid = -1;
    sigset_t mask;    
    
    char name [MAXNAME];
    info * buffer;
    if(argc!=2){
        printf("Numero de parametros incorrectos\n");
        exit(EXIT_FAILURE);
    }
    n= atoi(argv[1]);
    /*Geneamos clave para IPC */
    key=ftok(FILEKEY, KEY);
    if(key==-1){
        perror("No se genero una clave correctamente");
        exit(EXIT_FAILURE);
    }   
    
  
    /*Creamos zona de memoria compartida*/
    id_zone = shmget(key, sizeof(info), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone==-1){
        perror("No se pudo crear la zona de memoria de compartida");
        exit(EXIT_FAILURE);
    } 
    /*Unimos la zona de memoria compartida con nuestra variable buffer*/
    buffer = shmat(id_zone, (char *)0 , 0);
    if(buffer==NULL){
        perror("No se pudo unir la memoria");
        exit(EXIT_FAILURE);
    }
    /*El padre lanza n hijos*/
    for(i=0;i<n;i++){
        childpid=fork();
        if(childpid==-1){
            perror("Error al lanazar proceso hijo");
            exit(EXIT_FAILURE);
        /*Los hijos no lanzan mas procesos a su vez, por lo tanto salen del bucle*/
        } else if (childpid==0){
            break;
        } 
    }

    /*Padre*/
    if(childpid>0){
        /*Armamos el manejador para la sennal SIG_USR1 solo para el padre*/
        if(signal(SIGUSR1, manejador_SIGUSR1)==SIG_ERR){
            perror("Error al armar el manejador");
            exit(EXIT_FAILURE);
        }
        if(signal(SIGINT, manejador_SIGINT)==SIG_ERR){
            perror("Error al armar el manejador");
            exit(EXIT_FAILURE);
        }
        /*Mascara para esperar a la sennal SIGUSR1*/
        if(sigfillset(&mask)==-1){
            perror("Error en la mascara");
            exit(EXIT_FAILURE);
        }
        if(sigdelset(&mask,SIGUSR1)==-1){
            perror("Error en la mascara");
            exit(EXIT_FAILURE);
        }
        /*Se permite la sennal SIGINT para permitir al usuario terminar (interrumpir)
         el programa si es necesario*/
        if(sigdelset(&mask,SIGINT)==-1){
            perror("Error en la mascara");
            exit(EXIT_FAILURE);
        }           
        
        for (i=0; i<n; i++){
            /*Esperamos a que un hijo mande una solicitud (una sennal) de alta de un
            cliente*/
            sigsuspend(&mask);
            /*Imprimimos la zona de memoria compartida*/
            strcpy(name,buffer->nombre);
            name[strlen(name)-1]='\0';
            printf("Se ha dado de alta al cliente %s con id: %d\n",name,
                buffer->id);
        }
        /*Liberamos la zona de memoria compartida*/
        shmdt((char *) buffer);
        shmctl (id_zone, IPC_RMID, ( struct shmid_ds *)NULL);
        /*Esperamos a TODOS los hijos*/
        while(ret>0){
            ret=wait(NULL);
        }
        exit(EXIT_SUCCESS);

    } else if (childpid==0){

        /*Codigo del Hijo*/
    
        /*Seed*/
        srand(getpid() + time(NULL));
        /*Duerme un tiempo aleatorio entre 0 y MAXSLEEP segundos*/
        sleep(rand()%MAXSLEEP);
        buffer->id++;
        /*Solicita el nombre de un cliente al usuario*/
        printf("\n\nIntroduzca el nombre del cliente: ");
        fgets(name, MAXNAME, stdin);
        strcpy(buffer->nombre,name);
        
        

        /*Manda sennal al proceso padre*/
        kill(getppid(),SIGUSR1);
        /*Se desprende de la zona compartida*/
        shmdt((char *) buffer);

        /*Termina la ejecucion del hijo*/
        exit(EXIT_SUCCESS);
    
    
    }

}
/*Manejador de la sennal SIGUSR1*/
void manejador_SIGUSR1(int sig){
    
    if(signal(SIGUSR1, manejador_SIGUSR1)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }
}
/*Manejador de la sennal sigint, libera todos  los recursos*/
void manejador_SIGINT(int sig){

    int key=ftok(FILEKEY, KEY);
    char command[40];


    sprintf(command, "ipcrm -M %d",key );
    system(command);
    kill(-1*getpid(),SIGKILL);
}
