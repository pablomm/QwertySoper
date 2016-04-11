/**
 * Ejercicio 6 de la practica 3
 * Sistemas Operativos 
 * @brief Ejercicio 6
 * @file semaforos.c
 * @author Pablo Marcos Manchon pablo.marcosm@estudiante.uam.es
 * @author David Nevado Catalan david.nevadoc@estudiante.uam.es
 * @date 11/04/2016
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "semaforos.h"

#define NUM_COCHES 100
#define PUENTE 2
#define T_ESPERA 20
#define T_CRUZAR 1

/**
 * @brief key para generar los semaforos
 */
#define SEMKEY 75798

/**
 * @brief key para generar la memoria compartida
 */
#define SHMKEY 70234

/**
 * Funcion cruzar puente, se encarga de que el coche espere
 * y regula el paso al puente
 * @brief Funcion cruzar puente
 * @file semaforos.c
 * @date 11/04/2016
 */
int cruzarPuente(int direccion,int nCoche, int semid, int *contador);

/**
 * Funcion encargada de aumentar el contador del puente, y si
 * es el primer coche hace down del puente
 * @brief Regula entrada al puente
 * @file semaforos.c
 * @date 11/04/2016
 */
int lightSwitchOn(int semid,int direccion,int *contador);

/**
 * Funcion encargada de decrementar el contador del puente, y si
 * es el ultimo coche hace up del puente
 * @brief Regula salida al puente
 * @file semaforos.c
 * @date 11/04/2016
 */
int lightSwitchOff(int semid,int direccion,int *contador);

/**
 * Main ejercicio 6 
 * @brief Main ejercicio 6
 * @file semaforos.c
 * @author Pablo Marcos Manchon pablo.marcosm@estudiante.uam.es
 * @author David Nevado Catalan david.nevadoc@estudiante.uam.es
 * @date 11/04/2016
 */
int main(void){

    int i,ret,ppid;
    int semid,shmid;
    int *datos;
    unsigned short semaf[3] = {1,1,1};

    ppid = getppid();
    /* Creamos 3 semaforos, dos mutex para contadores y otro para el puente */
    if(Crear_Semaforo(SEMKEY, 3, &semid) == ERROR){
        perror("Error al crear los semaforos");
        exit(EXIT_FAILURE);
    }

    Inicializar_Semaforo(semid, semaf);

    /* Creamos memoria compartida para 2 contadores */
    if((shmid=shmget(SHMKEY, 2 * sizeof(int),IPC_CREAT | SHM_R | SHM_W)) == ERROR ){
        perror("Error creando memoria compartida");
        Borrar_Semaforo(semid);
        exit(EXIT_FAILURE);
    }   
    /* Obtenemos la memoria compartida */
    if((datos= (int*) shmat(shmid,(char*)0,0)) == NULL){
        perror("Error obteniendo memoria compartida");
        Borrar_Semaforo(semid);
        exit(EXIT_FAILURE);
    }

    /* Bucle para crear procesos */
    for(i=0; i < NUM_COCHES; i++){
        switch(fork()){
            case -1: /* Caso de error */
                perror("Error en el fork");
                /* Esperamos posibles hijos antes de acabar */
                if(ppid == getpid()){                
                    ret = 1;
                    while(ret){
                        ret = wait(NULL);
                    }
                }
                exit(EXIT_FAILURE);

            case 0: /* Caso del hijo */
            /* Intercalamos direcciones */
                if(cruzarPuente(i%2 ,i, semid, &datos[i%2]) == ERROR){
                    perror("Error al cruzar el puente");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
        } 
    }
    /* Esperamos a todos los hijos */
    for(i=0; i < NUM_COCHES; i++){
        wait(NULL);
    }

    /* Borramos semaforo y memoria compartida */
    Borrar_Semaforo(semid);
    shmdt((char*)datos);
    shmctl(shmid,IPC_RMID,(struct shmid_ds*)NULL);

    exit(EXIT_SUCCESS);
}

int cruzarPuente(int direccion,int nCoche, int semid, int *contador){
    /* Esperamos para cruzar */
    srand(getpid());
    sleep((rand() % T_ESPERA) + 1);

    if(lightSwitchOn(semid,direccion,contador) == ERROR){
        return ERROR;
    }

    /* Cruzamos el puente */
    printf("PID %d - Coche %3d cruzando direccion %s\n",getpid(),nCoche, (!direccion) ? "norte" : "sur");
    sleep(T_CRUZAR);
    printf("PID %d - Coche %3d ha terminado direccion %s\n",getpid(),nCoche, (!direccion) ? "norte" : "sur");
    
    if(lightSwitchOff(semid,direccion,contador) == ERROR){
        return ERROR;
    }

    return OK;
}

int lightSwitchOn(int semid,int direccion,int *contador){

    /* Zona critica del contador */
    if(Down_Semaforo(semid, direccion, 0) == ERROR){
        return ERROR;
    }
    /* Aumentamos el contador */
    (*contador)++;

    if((*contador) == 1){ /* El primero hace down del puente */
         if(Down_Semaforo(semid, PUENTE, 0) == ERROR){
            return ERROR;
        }
    }
    if(Up_Semaforo(semid, direccion, 0) == ERROR){
        return ERROR;
    }

    return OK;
}

int lightSwitchOff(int semid,int direccion,int *contador){

    /* Zona critica del contador */
    if(Down_Semaforo(semid, direccion, 0) == ERROR){
        return ERROR;
    }
    /* Decrementamos el contador */
    (*contador)--;
    if((*contador) == 0){ 
        /* El ultimo hace up del puente */
        printf("  - No hay coches en el puente -\n");
        if(Up_Semaforo(semid, PUENTE, 0) == ERROR){
            return ERROR;
        }
    }

    if( Up_Semaforo(semid, direccion, 0) == ERROR ){
        return ERROR;
    }

    return OK;
}
