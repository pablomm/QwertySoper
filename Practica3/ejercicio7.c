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
#include <pthread.h>
#include "semaforos.h"

#define NUM_COCHES 100
#define PUENTE 2
#define T_ESPERA 20
#define T_CRUZAR 1

#define _XOPEN_SOURCE 600

/**
 * @brief key para generar los semaforos
 */
#define SEMKEY 75798

/**
* @brief Estructura con datos para la funcion
* multiplicador utilizada por los hilos
*/
typedef struct _datos{
    /** Contador de coches */
    int *contador; 
    /** direccion del coche */
    int direccion;
    /** Identificador del semaforo */
    int semid; 
    /** Numero de hilo */
    int numCoche;
} datos;

/**
 * Funcion cruzar puente, se encarga de que el coche espere
 * y regula el paso al puente
 * @brief Funcion cruzar puente
 * @file semaforos.c
 * @date 11/04/2016
 */
void *cruzarPuente(datos *d);

/**
 * Funcion encargada de aumentar el contador del puente, y si
 * es el primer coche hace down del puente
 * @brief Regula entrada al puente
 * @file semaforos.c
 * @date 11/04/2016
 */
int lightSwitchOn(int semid,int direccion, int *contador);

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

    int i;
    int semid;
    pthread_t hilo[NUM_COCHES];
    datos data[NUM_COCHES];
    int contador[2] = {0,0};
    unsigned short semaf[3] = {1,1,1};

    /* Semilla para numeros aleatorios */
    srand(getpid());

    /* Creamos 3 semaforos, dos mutex para contadores y otro para el puente */
    if(Crear_Semaforo(SEMKEY, 3, &semid) == ERROR){
        perror("Error al crear los semaforos");
        exit(EXIT_FAILURE);
    }
    /* Lo inicializamos */
    Inicializar_Semaforo(semid, semaf);

    

    /* Bucle para crear hilos*/
    for(i=0; i < NUM_COCHES; i++){
        
        /* Inicializamos la estructura asociada al proceso */  
        data[i].contador = &(contador[i%2]);
        data[i].direccion = i%2;
        data[i].semid = semid;
        data[i].numCoche = i;
        pthread_create(&(hilo[i]),NULL,(void * (*)(void *))cruzarPuente,&(data[i]));
        
    }
    /* Esperamos a todos los hilos */
    for(i=0; i < NUM_COCHES; i++){
        pthread_join(hilo[i],NULL);
    }

    /* Borramos los semaforos */
    if(Borrar_Semaforo(semid) == ERROR){
        perror("Error al borrar semaforos");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void *cruzarPuente(datos *d){
    
    /* Esperamos para cruzar */
    sleep((rand() % T_ESPERA) + 1);

    /* Pasamos al puente */
    lightSwitchOn(d->semid,d->direccion,d->contador);

    /* Cruzamos el puente */
    printf("Coche %3d cruzando direccion %s\n",d->numCoche, (!d->direccion) ? "norte" : "sur");
    sleep(T_CRUZAR);
    printf("Coche %3d ha terminado direccion %s\n",d->numCoche, (!d->direccion) ? "norte" : "sur");
    
    /* Salimos del puente */ 
    lightSwitchOff(d->semid,d->direccion,d->contador);
    
    pthread_exit(NULL);
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
