/**
 * Pruebas de las bibliotecas de semaforos para
 * implementar las practicas de Sistemas Operativos
 * @brief Biblioteca de semaforos
 * @file ejercicio5.c
 * @author Pablo Marcos Manchon pablo.marcosm@estudiante.uam.es
 * @author David Nevado Catalan david.nevadoc@estudiante.uam.es
 * @date 11/04/2016
 */

#include "semaforos.h"
#include <assert.h>
#include <stdlib.h>

#define SEMKEY 77387
#define NUM_SEM 4



/**
 * Pruebas de las funciones de crear y borrar semaforos de la biblioteca
 * @brief Test 1 biblioteca de semaforos
 * @date 11/04/2016
 */
int CrearBorrar_test(){
    int semid;
    /*Primero crea con exito, despues ya esta creado*/
    if( Crear_Semaforo(SEMKEY,1, &semid)==0 &&  Crear_Semaforo(SEMKEY, 1,&semid)==1
        && Borrar_Semaforo(semid)==0){
        return 1;
    }
    return 0;

}
/**
 * Pruebas de la funcion de creacion multiple de semaforos de la biblioteca
 * @brief Test 2 biblioteca de semaforos
 * @date 11/04/2016
 */
int  Crear_Multiple_test(){
    int semid;
    /*Primero crea con exito, despues ya esta creado*/
    if( Crear_Semaforo(SEMKEY,NUM_SEM, &semid)==0 &&  Crear_Semaforo(SEMKEY, NUM_SEM,&semid)
        && Borrar_Semaforo(semid)==0){
        return 1;
    }
    return 0;

}
/**
 * Pruebas de las funcion inicializar semaforo de la biblioteca
 * @brief Test 3 biblioteca de semaforos
 * @date 11/04/2016
 */
int Inicializar_test(){
    int semid;
    unsigned short ini=1;

    if( Crear_Semaforo(SEMKEY,1, &semid)==0 && Inicializar_Semaforo(semid, &ini)==0
        && semctl(semid, 0, GETVAL)==1 && Borrar_Semaforo(semid)==0){
        return 1;
    }
    return 0;
}


/**
 * Pruebas de las funcion up y down de semaforos de la biblioteca
 * @brief Test 4 biblioteca de semaforos
 * @date 11/04/2016
 */



int Up_Down_test(){
    
    int semid;
    unsigned short ini;
    ini=2;
    
    if(Crear_Semaforo(SEMKEY,1, &semid)==0 && Inicializar_Semaforo(semid, &ini)==0
        && semctl(semid,0,GETVAL) ==2 && Down_Semaforo(semid, 0, SEM_UNDO)==0
        && semctl(semid,0,GETVAL) ==1 && Up_Semaforo(semid, 0, SEM_UNDO)==0
        && semctl(semid,0,GETVAL) ==2 && Up_Semaforo(semid, 0, SEM_UNDO)==0
        && semctl(semid,0,GETVAL) ==3 
        && Borrar_Semaforo(semid)==0){
        
        return 1;
    }
    return 0;
}

/**
 * Pruebas de las funcion up y down multiples de semaforos de la biblioteca
 * @brief Test 5 biblioteca de semaforos
 * @date 11/04/2016
 */



int Up_Down_Multiple_test(){
    int semid;
    unsigned short ini[NUM_SEM];
    int i;
    int ups[2];
    int downs[3];
    for(i=0;i<NUM_SEM;i++){
        ini[i]=2;
    }
    ups[0]= 0;
    ups[1]= 1;
    
    downs[0]=1;
    downs[1]=2;
    downs[2]=3;
    if(Crear_Semaforo(SEMKEY,NUM_SEM, &semid)==0 && Inicializar_Semaforo(semid, ini)==0
        && semctl(semid,0,GETVAL) ==2 && semctl(semid,1,GETVAL) ==2
        && semctl(semid,2,GETVAL) ==2 && semctl(semid,3,GETVAL) ==2
        && DownMultiple_Semaforo(semid,3, SEM_UNDO, downs)==0
        && UpMultiple_Semaforo(semid,2, SEM_UNDO, ups)==0
        && semctl(semid,0,GETVAL) ==3 && semctl(semid,1,GETVAL) ==2
        && semctl(semid,2,GETVAL) ==1 && semctl(semid,3,GETVAL) ==1
        && Borrar_Semaforo(semid)==0){
        return 1;
    }
    return 0;
}
/**
 * Pruebas de las funcion de semaforos de la biblioteca cuando dan error
 * @brief Test 6 biblioteca de semaforos
 * @date 11/04/2016
 */


int Errores_test(){
    int semid=0;
    int ups[2];
    int downs[3];

    ups[0]= 0;
    ups[1]= 1;
    
    downs[0]=1;
    downs[1]=2;
    downs[2]=3;
    if( Crear_Semaforo(SEMKEY,-1, &semid)==-1 && Crear_Semaforo(SEMKEY,-1, NULL)==-1
         && Borrar_Semaforo(semid)==-1 && Crear_Semaforo(SEMKEY,1, &semid)==0
         && Inicializar_Semaforo(semid,NULL)==-1 &&
         Down_Semaforo(semid,-1,SEM_UNDO)==-1 && Up_Semaforo(semid, -1, SEM_UNDO)==-1
         && DownMultiple_Semaforo(semid,3, SEM_UNDO, downs)==-1
         && UpMultiple_Semaforo(semid,2, SEM_UNDO, ups)==-1
         && Borrar_Semaforo(semid)==0){
        return 1;
    }
    return 0;
}
/**
 * @brief Main que ejecuta todos los tests de la biblioteca de semaforos
 * @return EXIT_SUCCESS si todos los tests se ejecutan con exito
 */
int main(){
    assert(CrearBorrar_test());
    printf("1.Funciones de creacion y borrado comprobadas\n");
    assert(Crear_Multiple_test());
    printf("2.Funcion de creacion multiple comprobada\n");
    assert(Inicializar_test());
    printf("3.Funcion de inicializacion comprobada\n");
    assert(Up_Down_test());
    printf("4.Funciones Up y Down simples comprobadas\n");
    assert(Up_Down_Multiple_test());
    printf("5.Funciones Up y Down multiples comprobadas\n");
    assert(Errores_test());
    printf("6.Funciones en caso de error comprobadas\n");
    printf("Tests pasados con exito\n");
    exit(EXIT_SUCCESS);

}

