/**
* @brief Ejercicio 9.
* Contiene un programa que lanza 2 hijos, estos lanzan a
* a su vez 2 hijos mas. Despues mediante tuberias, el padre envia
* un mensaje a sus nietos. Estos lo imprimen por pantalla junto
* con el PID del proceso por donde lo recibieron y mandan una
* respuesta. Estas respuestas son recibidas por el padre, que las
* imprime por pantalla junto con el PID del nieto que las mando.
*
* @file ejercicio9.c
* @author David Nevado Catalan <david.nevadoc@estudiante.uam.es>
* @author Pablo Marcos Manchon <pablo.marcosm@estudiante.uam.es>
* @version 1.0
* @date 23-02-2016
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define TAM 150

/**
* @brief main del programa del ejercicio 9
* @return EXIT_SUCCESS si fue todo bien o EXIT_FAILURE en caso contrario
*/
int main(void){
    int i=0;
    int j=0;
    int status=0;
    /*Las tuberias con cuyo segundo indice es
        0 comunican datos de padres a hijos (se llamaran de bajada)
        1 comunican datos de hijos a padres (se llamaran de subida)*/

    /*Tuberias de padre a hijos*/
    int fd1[2][2]; /*Pipe handle 1*/
    int fd2[2][2]; /*Pipe handle 2*/
    /*Tuberias de hijos a nietos*/
    int fd3[2][2]; /*Pipe handle 3*/
    int fd4[2][2]; /*Pipe handle 4*/

    pid_t childpid1 = -1; /*PID del hijo 1 del padre*/
    pid_t childpid2 = -1; /*PID del hijo 2 del padre*/
    pid_t childpid3 = -1; /*PID del hijo 1 de uno de los hijos*/
    pid_t childpid4 = -1; /*PID del hijo 1 de uno de los hijos*/

    /*Variables para almacenar los mensajes entre procesos*/

    char buf[TAM]; /*Variable que usaran los primeros hijos para almacenar
el contenido recibido por las tuberias de su padre*/
    char buf2[TAM]; /*Variable que usaran los nietos para almacenar
el contendio recibido por las tuberias de su padre (los hijos) */

    char respuestas1[2][TAM];/*Variables que utilizaran los primeros hijos para
almacenar el contenido recibido por las tuberias de sus hijos (los nietos)*/
    char respuestas2[2][TAM];/*Variables que utilizara el padre para  almacenar
el contenido recibido por las tuberias de sus hijos (los hijos)*/

    /*Mensaje que se envia desde el padre hacia los nietos*/
    char mensaje[] = "Datos enviados a traves de la tuberia";
    /*Respuesta que se envia desde los nietos hacia el padre*/
    char respuesta[]="Datos devueltos a través de la tubería por el proceso PID=";

    /*Tuberias de bajada*/
    /*Control de error de la creacion de la tuberia 1.0*/
    if(pipe(fd1[0])==-1){
        perror("Error al crear la tuberia 1\n");
        exit(EXIT_FAILURE);
    }
    /*Control de error de la creacion de la tuberia 2.0*/
    if(pipe(fd2[0])==-1){
        perror("Error al crear la tuberia 2\n");
        exit(EXIT_FAILURE);
    }
    /*Tuberias de subida*/
    /*Control de error de la creacion de la tuberia 1.1*/
    if(pipe(fd1[1])==-1){
        perror("Error al crear la tuberia 1\n");
        exit(EXIT_FAILURE);
    }
    /*Control de error de la creacion de la tuberia 2.1*/
    if(pipe(fd2[1])==-1){
        perror("Error al crear la tuberia 2\n");
        exit(EXIT_FAILURE);
    }

    childpid1=fork();
    /*Control de error de la creacion del proceso hijo 1*/
    if(childpid1==-1){
        perror("Error al crear proceso hijo 1\n");
        exit(EXIT_FAILURE);
    }

    if(childpid1>0){
        /*El proceso padre abre un nuevo proceso*/
        /*Control de error de la creacion del proceso hijo 2*/
        childpid2=fork();
        if(childpid2==-1){
            perror("Error al crear proceso hijo 2\n");
            exit(EXIT_FAILURE);
        }
        /*Linea de ejercucion del proceso padre*/
        if(childpid2>0){
            /*El proceso padre cierra la entrada de las tuberias 1.0 y 2.0*/
            close(fd1[0][0]);
            close(fd2[0][0]);
            /*El proceso padre cierra la salida de las tuberias 1.1 y 2.1*/
            close(fd1[1][1]);
            close(fd2[1][1]);
            /*El proceso padre envia el mensaje por las tuberias*/
            write(fd1[0][1], mensaje, strlen(mensaje)+1);
            write(fd2[0][1], mensaje, strlen(mensaje)+1);
            /*El proceso padre lee la respuesta enviada por sus hijos*/
            read(fd1[1][0],respuestas2[0],TAM);
            read(fd2[1][0],respuestas2[1],TAM);
            /*El proceso padre imprime las respuestas*/
            printf("PADRE: %s%s", respuestas2[0],respuestas2[1]);
            /*El proceso padre espera a que sus dos hijos terminen*/
            wait(&status);
            wait(&status);
            /*Fin del programa*/
            exit(EXIT_SUCCESS);
        }
    }
    /*Linea de ejecucion de los procesos hijos */
    /*Hijo 1*/
    if (childpid1==0){
        /*Cierra las tuberias 2.0 y 2.1, y la salida de la tuberia 1.0 y la
          entrada de 1.1*/
        for(i=0;i<2;i++){
            for(j=0;j<2;j++){
            close(fd2[i][j]);
            }
        }
        close(fd1[0][1]);
        close(fd1[1][0]);
        /*Lee el contenido enviado por la tuberia 1*/
        read(fd1[0][0], buf, TAM );
    }
    /*Hijo 2*/
    else if (childpid2==0){
        /*Cierra las tuberias 1.0 y 1.1, y la salida de la tuberia 2.0 y la
          entrada de 2.1*/
        for(i=0;i<2;i++){
            for(j=0;j<2;j++){
            close(fd1[i][j]);
            }
        }
        close(fd2[0][1]);
        close(fd2[1][0]);
        /*Lee el contenido enviado por la tuberia 2*/
        read(fd2[0][0], buf, TAM );
    }
    /*Ambos procesos abren tuberias para comunicarse con sus hijos*/
    /*Tuberias de bajada*/
    /*Control de error de la creacion de la tuberia 3.0*/
    if(pipe(fd3[0])==-1){
        perror("Error al crear la tuberia 3.0\n");
        exit(EXIT_FAILURE);
    }
    /*Control de error de la creacion de la tuberia 4.0*/
    if(pipe(fd4[0])==-1){
        perror("Error al crear la tuberia 4.0\n");
        exit(EXIT_FAILURE);
    }
    /*Tuberias de subida*/
    /*Control de error de la creacion de la tuberia 3.1*/
    if(pipe(fd3[1])==-1){
        perror("Error al crear la tuberia 3.1\n");
        exit(EXIT_FAILURE);
    }
    /*Control de error de la creacion de la tuberia 4.1*/
    if(pipe(fd4[1])==-1){
        perror("Error al crear la tuberia 4.1\n");
        exit(EXIT_FAILURE);
    }
    /*Ambos procesos lanzaran ahora 2 hijos mas*/
    /*Control de error de la creacion del proceso hijo 3*/
    childpid3=fork();
    if(childpid3==-1){
        perror("Error al crear proceso hijo 3\n");
        exit(EXIT_FAILURE);
    }
    if(childpid3>0){
        /*Control de error de la creacion del proceso hijo 4*/
        childpid4=fork();
        if(childpid4==-1){
            perror("Error al crear proceso hijo 4\n");
            exit(EXIT_FAILURE);
        }
        if(childpid4>0){
            /*El proceso hijo directo del padre cierra la entrada de las tuberias 3.0 y 4.0 y la salida de las 3.1 y 4.1*/
            close(fd3[0][0]);
            close(fd4[0][0]);
            close(fd3[1][1]);
            close(fd4[1][1]);
            /*El proceso obtiene su PID y lo agrega al mensaje*/
            sprintf(buf,"%s por el proceso PID= %d", buf, getpid());
            /*Envia el mensaje por las tuberias a sus hijos*/
            write(fd3[0][1],buf,strlen(buf)+1);
            write(fd4[0][1],buf,strlen(buf)+1);
             /*El proceso lee la respuesta enviada por sus hijos*/
            read(fd3[1][0],respuestas1[0],TAM);
            read(fd4[1][0],respuestas1[1],TAM);
            /*El proceso junta las respuestas y las envia a su padre*/
            sprintf(buf,"%s%s",respuestas1[0],respuestas1[1]);
            if(childpid1==0){
            write(fd1[1][1], buf, strlen(buf)+1);
            } else {
            write(fd2[1][1], buf, strlen(buf)+1);
            }
            /*Espera a que sus hijos terminen*/
            wait(&status);
            wait(&status);
            /*Termina su ejecucion*/
            exit(EXIT_SUCCESS);
        }
    }
     /*Linea de ejecucion de los procesos hijos (segundos) */
    /*Hijo 3*/
    if (childpid3==0){
        /*Cierra las tuberias 4.0 y 4.1, y la salida de la tuberia 3.0 y la
entrada de 3.1*/
         for(i=0;i<2;i++){
            for(j=0;j<2;j++){
            close(fd4[i][j]);
            }
        }
        close(fd3[0][1]);
        close(fd3[1][0]);
        /*Lee el contenido enviado por la tuberia 3*/
        read(fd3[0][0], buf2, TAM );
        /*Imprime el mensaje leido*/
        printf("NIETO: %s\n", buf2);
        /*Envia el mensaje de respuesta a su padre*/
        sprintf(buf2,"%s%d\n", respuesta, getpid());
        write(fd3[1][1],buf2, strlen(buf2)+1);
        /*Termina su ejecucion*/
        exit(EXIT_SUCCESS);
    }
    /*Hijo 4*/
    else if (childpid4==0){
        /*Cierra las tuberias 3.0 y 3.1, y la salida de la tuberia 4.0 y la
entrada de 4.1*/
         for(i=0;i<2;i++){
            for(j=0;j<2;j++){
            close(fd3[i][j]);
            }
        }
        close(fd4[0][1]);
        close(fd4[1][0]);
        /*Lee el contenido enviado por la tuberia 2*/
        read(fd4[0][0], buf2, TAM );
        /*Imprime el mensaje leido*/
        printf("NIETO: %s\n", buf2);
        /*Envia el mensaje de respuesta a su padre*/
        sprintf(buf2,"%s%d\n", respuesta, getpid());
        write(fd4[1][1],buf2, strlen(buf2)+1);
        /*Termina su ejecucion*/
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}
