/**
* @brief Modulo con main y funciones del ejercicio 4
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @file ejercicio4.c
* @date 2016/03/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

/**
* @brief Dimension de las matrices
*/
#define DIM 3

/**
* @brief Numero de matrices/hilos
*/
#define NUM 2

/**
* @brief Tamanno maximo a leer al cargar matriz
*/
#define MAX (10*DIM*DIM)

/**
* @brief Retardo entre filas en nanosegundos
*/
#define RETARDO 500000

/**
* @brief Retardo en la creacion de hilos
*/
#define RETARDO_HILOS 250000


/**
* @brief Estructura con datos para la funcion
* multiplicador utilizada por los hilos
*/
typedef struct _datos{
    /** Matriz almacenada */
    int matrix[DIM][DIM]; 
    /** Multiplicador */
    int mult; 
    /** Numero de hilo */
    int hilo; 
    /** Filas de todos los hilos */
    int *fila; 

} datos;

/**
* @brief Funcion para multilicar matriz
* @params datos* estructura con datos de la matriz
*/
void *multiplicador(datos *m){
    int j;

    /* Bucle multiplica filas */
    for(m->fila[m->hilo] = 0; m->fila[m->hilo] < DIM; m->fila[m->hilo]++){
        usleep(RETARDO); /* Introducimos el retardo */ 
  
        for(j=0;j<DIM;j++){ /* Bucle multiplica columnas */
            m->matrix[m->fila[m->hilo]][j] *= m->mult;
        }
        /* Imprimimos resultado de la fila */
        printf("Hilo %d multiplicando fila %d resultado",m->hilo+1,m->fila[m->hilo]);

        for(j=0; j<DIM; j++){ /* Imprimimos cada elemento */
            printf(" %d",m->matrix[m->fila[m->hilo]][j]);
        }
        for(j=0;j<NUM; j++){ /* Imprimimos filas de otros hilos */
            if(j!=m->hilo){ /* Solo hilos diferentes */
                if(m->fila[j] < DIM){
                    printf(" - El hilo %d va por la fila %d",j+1,m->fila[j]); 
                } else {
                    printf(" - El hilo %d ha terminado",j+1);
                }
            }
        }
        printf("\n");
    }
    m->fila[m->hilo]++;
    pthread_exit(NULL);
}

/**
* El programa solicita la introducion de NUM matrices de dimension
* DIM y los multiplicadores correspondientes, lanza un hilo por matriz 
* que va multiplicando las filas, imprimiendolo por pantalla y la fila 
* por la que van el resto de los hilos
* @brief Main del ejercicio 4
*/
int main(void){
    datos matrix[NUM];
    char buff[MAX];
    char *parseNum;
    pthread_t hilo[NUM];
    int i,j,k;
    int fila[NUM];

    fprintf(stdout,"Multiplicador de matrices %dx%d\n",DIM,DIM);

    for(k=0;k<NUM;k++){ /* Bucle para leer las matrices */
    
        printf("Introduzca el multiplicador %d:\n",k+1);
        scanf("%d", &(matrix[k].mult));

        fprintf(stdout,"Introduzca la matriz %d:\n",k+1);
        getc(stdin);
        fgets(buff,MAX,stdin);

        for(i=0;i<DIM;i++){ /* Bucle para leer filas */
            for(j=0;j<DIM;j++){ /* Bucle para leer columnas */

                if(i==0 && j==0){
                    parseNum = strtok(buff," \t");

                    if(!parseNum){ /* Control de errores */
                        perror("Matriz introducida incorrecta");
                        exit(EXIT_FAILURE);
                    }
                    matrix[k].matrix[i][j] = atoi(parseNum);

                } else {
                    parseNum = strtok(NULL," \t");
                    if(!parseNum){ /* Control de errores */
                        perror("Matriz introducida incorrecta");
                        exit(EXIT_FAILURE);
                    }
                    matrix[k].matrix[i][j] = atoi(parseNum);
                }
            }
        }
        /* Introducimos en la estructura el resto de datos */
        matrix[k].hilo = k;
        fila[k] = 0;
        matrix[k].fila = fila;
    }

    for(i=0; i<NUM; i++){ /* Bucle para crear los hilos */
        pthread_create(&(hilo[i]),NULL,(void * (*)(void *))multiplicador,&(matrix[i]));
        usleep(RETARDO_HILOS);
    }

    for(i=0; i<NUM; i++){ /* Bucle para esperar los hilos */
        pthread_join(hilo[i],NULL);
    }
    exit(EXIT_SUCCESS);
}
