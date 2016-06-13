/**
 * Practicas Sistemas Operativos
 * Practica 4 - Ejercicio 2
 * @brief Implementacion de un gestor de examenes
 * @author Pablo Marcos Manchon
 * @author David Nevado Catalan
 * @file gestor_examen.c
 * @date 27/4/2016
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
#include <unistd.h>
#include "semaforos.h"
/**
 * @brief Clave para zona de memoria
 */
#define KEY 31415
/**
 * @brief Clave para zona de memoria
 */
#define KEY2 78840

/**
 * @brief Clave de fichero para zona de memoria
 */
#define FILEKEY "/bin/ls"
/**
 * @brief Numero de profesores lanzados
 */
#define N_PROFESORES 6
/**
 * @brief Tiempo en el que el profesor en recoger los examenes
 */

#define TIEMPO_EXAMEN 300
/**
 * @brief Tiempo de actualizacion del fichero
 */
#define T_PS 30
/**
 * @brief Tiempo maximo de espera antes de pedir aula de un alumno
 */
#define T_ALUMNO 5
/**
 * @brief Tiempo que tarda el alumno en terminar el examen
 */
#define T_ALUMNO_EXAMEN 390
/**
 * @brief Tiempo minimo que tarda el alumno en realizar el examen
 */
#define T_MIN 10


/**
 * @brief Manejador de SIGALRM del gestor
 */
void manejadorGestor_SIGALRM(int signal);
/**
 * Estructura que representa un aula
 * @brief Estructura que representa un aula
 */
typedef struct _aula {
    int ocupados;
    int totales;
    int pids[];
} Aula;

/**
 * Estructura de los mensajes para la sincronizacion de los procesos
 * @brief Estructura de los mensajes
 */

typedef struct _mensaje{
    /**Id del mensaje */
    long id; 
    /** Tipo de mensaje */
    int pid;
    /** */
    int entrada;
    /** Texto del mensaje */
    char aviso[1];
} Mensaje;


/**
 * @brief Manejador de SIGUSR1 del profesor a la entrada del aula
 */
void manejadorProfEntrada_SIGUSR1(int signal);

/**
 * @brief Codigo del profesor a la entrada del aula
 */
void profesor_entrada(int semid,int msqid, Aula *aula, int n_aula);
/**
 * @brief Codigo del profesor a la salida del aula
 */
void profesor_salida(int semid,int msqid, Aula *aula, int n_aula);
/**
 * @brief Codigo del alumno
 */
void alumno(int msqid);
/**
 * @brief Funcion encargada de liberar todos los recursos utilizadas
 */
void liberarIPC(int msqid, int *shmid, int semid);
void actualiza_fichero2();
int msqid;
/**
 * Punto de entrada a la funcion. Codigo del gestor del examen, encargado
 * de lanzar los procesos necesarios asi como de gestionar los recursos
 * que estos usen y imprimir por pantalla informacion util para el usuario.
 * @brief Funcion principal, gestor del examen.
 */
int main(){


    int nPlazas[3];
    key_t clave[2];
    int semid,shmid[2];
    int i,j,ret = 1;
    Aula *aula[2];
    unsigned short ini[2] = {1,1};
    int pidProfesor[6];
    int max;
    Mensaje msg;
    int ultimaAula;
    int flag=0;

    /* Solicitamos asientos y alumnos, comprobando sus valores */
    do {
        printf("\nAsientos disponibles en el aula 1: ");
        scanf("%d",&(nPlazas[0]));

        printf("\nAsientos disponibles en el aula 2: ");
        scanf("%d",&(nPlazas[1]));

        if(nPlazas[0] <= 0 || nPlazas[1] <= 0){
            printf("\nNumero de asientos incorrectos");
        }
        
    } while (nPlazas[0] <= 0 || nPlazas[1] <= 0);

    do {
        printf("\nIntroduzca el numero de alumnos: ");
        scanf("%d",&(nPlazas[2]));

        max = nPlazas[0] < nPlazas[1] ? nPlazas[1] : nPlazas[0];

    } while(nPlazas[2] >= nPlazas[0] + nPlazas[1] || nPlazas[2] <= max);


    /* Generamos clave para obtener memoria y semaforos */
    //Aqui podriamos poner KEY KEY2 en un array y ahorra codigo con un for
    clave[0] = ftok(FILEKEY,KEY);
    if(clave[1] == -1){
        perror("Error generando key");
        exit(EXIT_FAILURE);
    }

    clave[1] = ftok(FILEKEY,KEY2);
    if(clave[1] == -1){
        perror("Error generando key");
        exit(EXIT_FAILURE);
    }

    /* Creamos los semaforos */
    if(Crear_Semaforo(clave[0], 2, &semid) == -1){
        perror("Error creando el semaforo");
        exit(EXIT_FAILURE);
    }

    /* Inicializamos los semaforos */
    if( Inicializar_Semaforo(semid,ini) == -1){
        perror("Error inicializando semaforo");
        exit(EXIT_FAILURE);

    }

    /* Creamos la cola de mensajes */
    msqid = msgget (clave[0], 0660 | IPC_CREAT);
    if (msqid == -1){
        perror("Error al crear identificador la para cola mensajes");
        exit(EXIT_FAILURE);
    }


    /* Reservamos memoria compartida para las aulas */

    for(i=0; i<2;i++){
        
        shmid[i]=shmget(clave[i],sizeof(Aula)+sizeof(int)*(nPlazas[i]), IPC_CREAT | 0660);
        
        if(shmid[i]==-1){
            perror("Error abriendo memoria");
            exit(EXIT_FAILURE);
        }
        aula[i]=shmat(shmid[i],(char*)0,0);
        if(aula[i]==NULL){
            perror("Error abriendo memoria");
            exit(EXIT_FAILURE);
        }

        aula[i]->totales = nPlazas[i];
        aula[i]->ocupados = 0;

        for(j=0; j<nPlazas[i];j++){
            aula[i]->pids[j] = 0;
        }
    }
/**********************************Empezamos a lanzar procesos****************************/
    /* Lanzamos profesor entregan examenes */

    for(i=0; i< 4; i++){
        pidProfesor[i] = fork();
        switch(pidProfesor[i]){
            case -1:
                perror("Error en el fork");
                /* Padre librea ipc <------------------ falta*/
                exit(EXIT_FAILURE);
            case 0:
            profesor_entrada(semid,msqid,aula[i%2],i%2 + 1);
            printf("Asignado profesor de entrada al aula %d - PID: %d\n",i%2+1,getpid());
            exit(EXIT_SUCCESS);

        }
    }

    /* Lanzamos profesor recogen examen (2) */
    for( ; i < 6; i++){
        pidProfesor[i] = fork();
        switch(pidProfesor[i]){
            case -1:
                perror("Error en el fork");
                exit(EXIT_FAILURE);
            case 0:
                /* Funcion profesor */
            printf("Asignado profesor de recogida al aula %d - PID: %d\n",i%2+1,getpid());
            profesor_salida(semid,msqid,aula[i%2],i%2 + 1);

            exit(EXIT_SUCCESS);
        }
    }

    /* Lanzamos alumnos */
    for(i=0 ; i < nPlazas[2]; i++){
        pidProfesor[i] = fork();
        switch(pidProfesor[i]){
            case -1:
                perror("Error en el fork");
                exit(EXIT_FAILURE);
            case 0:
                /* Funcion profesor */
             alumno( msqid);
			//liberarIPC(msqid,shmid,semid);
            exit(EXIT_SUCCESS);

        }
    }
    printf("Gestor: todos los alumnos y profesores han sido lanzados\n");

    if(signal(SIGALRM,manejadorGestor_SIGALRM)==SIG_ERR){
        perror("Error armando manejador");
		exit(EXIT_FAILURE);
    }

    /*if(signal(SIGUSR2,manejadorGestor_SIGUSR2)==SIG_ERR){
        perror("Error armando manejador");
		exit(EXIT_FAILURE);
    }*/

  if(signal(SIGTERM, SIG_IGN)==SIG_ERR){
        perror("Error al armar el manejador del gestor");
        exit(EXIT_FAILURE);
	}

    /* Iniciamos gestion de procesos */

    /* Lanzamos alarma encargada de generar archivo ps */    
    alarm(T_PS);
    srand(getpid());
    
    while(1){ /* Recepcion de alumnos */
    
        ultimaAula = rand()%2; /* Asignamos aleatoriamente nueva aula */
        /* Esperamos a que un alumno quiera entrar a un aula */
        msgrcv(msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), 3, 0);
        
        if(msg.entrada == 666){
            flag++;
            if(flag > 1)break;
        }

        if(msg.entrada == -1)continue; /* Caso desbloqueado por senal */

        /* Miramos si la ocupacion de la primera aula es < 85% */
        Down_Semaforo(semid, ultimaAula , 0);
             if(aula[ultimaAula]-> ocupados * 100 / aula[ultimaAula] -> totales < 85){
                aula[ultimaAula] -> ocupados++;
                Up_Semaforo(semid, ultimaAula , 0);

                /* Mandamos mensaje al aula */

                msg.id = ultimaAula + 1;
                msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
            
            } else { /* Si es >85% intentamos mandarlo al otro aula */
                Up_Semaforo(semid, ultimaAula , 0);
                ultimaAula = ultimaAula == 0 ? 1 : 0;

                Down_Semaforo(semid, ultimaAula , 0);
                if(aula[ultimaAula]-> ocupados == aula[ultimaAula] -> totales){ /* Si esta llena hay que mandarlo al primer aula */
                    Up_Semaforo(semid, ultimaAula , 0);
                    ultimaAula = ultimaAula == 0 ? 1 : 0;

                    Down_Semaforo(semid, ultimaAula , 0);
                    aula[ultimaAula] -> ocupados++;
                    Up_Semaforo(semid, ultimaAula , 0);

                    msg.id = ultimaAula + 1;

                } else {
                    aula[ultimaAula] -> ocupados++;
                    Up_Semaforo(semid, ultimaAula , 0);

                    msg.id = ultimaAula + 1;
                }
                msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
            }

        msg.id = 30;
        msg.pid = -1;
        msg.entrada = -1;
    }

   

    kill((-1)*getpid(), SIGTERM);
    
    /* Borramos IPC usados y esperamos hijos */
    printf("Esperamos a los hijos y liberamos IPC\n");
    for(i=0; i< N_PROFESORES + nPlazas[2]; i++){
        wait(NULL);
    }


    liberarIPC(msqid, shmid,semid);

    exit(EXIT_SUCCESS);
}

/**********************************************************************************/
/*****************************fin main********************************************/


void liberarIPC(int msqid, int *shmid, int semid){
    char cmd[20];
	int i;
	int ids[]={shmid[0],shmid[1], msqid, semid};
	char flag[]={'m','m','q','s'};
	for(i=0;i<4;i++){
		sprintf(cmd,"ipcrm -%c %d", flag[i], ids[i] );
		system(cmd);

	}
}




void manejadorGestor_SIGALRM(int sig){  
    actualiza_fichero2();
    alarm(T_PS);
	/*Rearmamos el manejador	 */
	if(signal(SIGALRM, manejadorGestor_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador del gestor");
        exit(EXIT_FAILURE);
	}
}





/************************************PROFESORES***************/


void manejadorProfesor_SIGALRM(int signal){
    Mensaje msg;
    printf("Profesor pid: %d - It's the final countdown!!\n",getpid());
    /*Le decimos al gestor que eche a todo el mundo*/
    /*kill(getppid(),SIGUSR2);*/
    msg.pid = -1;
    msg.entrada = 666;
    msg.id = 5 + 1;
    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
    msg.id = 5 + 2;
    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);


}




void profesor_salida(int semid,int msqid, Aula *aula, int n_aula){
    Mensaje msg;
    int i;

    if(signal(SIGALRM, manejadorProfesor_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador de SIGALRM en prfesor_salida");
        exit(EXIT_FAILURE);

    }

    alarm(TIEMPO_EXAMEN);
    
    while(1){

          msg.id = 30;
          msg.pid = -1;
          msg.entrada = -1;

          msgrcv(msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), n_aula + 5, 0);
          if(msg.entrada == 1) { /* Caso alumno entrega examen */

            printf("Profesor %d salida aula %d - Recogiendo examen a alumno %d\n",getpid(),n_aula,msg.pid);
                
            Down_Semaforo(semid, n_aula -1, 0);
                
            aula->ocupados--;
                /* Asignamos el asiento libre */            
            for(i=0;i<aula->totales;i++){
                if(aula->pids[i] == msg.pid){
                    aula->pids[i] = 0;
                    break;
                }
            }
            Up_Semaforo(semid, n_aula -1, 0);

         } if (msg.entrada == 666){
            Down_Semaforo(semid, n_aula -1, 0);
                
                /* Asignamos el asiento libre */            
            for(i=0;i<aula->totales;i++){
                if(aula->pids[i] != 0){
                    kill(aula->pids[i],SIGTERM);
                }
            }
            Up_Semaforo(semid, n_aula -1, 0);
        printf("Profesor %d salida aula %d - A casa todos los alumnos\n",getpid(),n_aula);

        msg.id = 3;
        msg.pid = -1;
        msg.entrada = 666;
        msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
        exit(EXIT_SUCCESS);
        }

    }

}


void profesor_entrada(int semid,int msqid, Aula *aula, int n_aula){
    int i;
    Mensaje msg;


    while(1){
        /* Espera a que un alumno quiera entrar */

        msg.id = 30;
        msg.pid = -1;
        msg.entrada = -1;

        msgrcv(msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), n_aula, 0);

        /* printf("Profesor %d - Mensaje recibido id %ld, pid %d , entrada %d\n",getpid(),msg.id,msg.pid,msg.entrada); */

        if(msg.entrada == 0){ /* Caso alumno intenta entrar */
            
            Down_Semaforo(semid, n_aula - 1, 0);
            printf("Profesor %d entrada aula %d - Asignando asiento a alumno %d\n",getpid(),n_aula,msg.pid);

            /* Asignamos el asiento libre */            
            for(i=0;i<aula->totales;i++){
                if(aula->pids[i] == 0){
                    aula->pids[i] = msg.pid;
                    break;
                }
            }
            Up_Semaforo(semid, n_aula -1, 0);
            /* Mandamos mensaje al alumno */
            msg.id = msg.pid;        /* ID del alumno */
            msg.pid = i;             /* Numero de sitio del alumno */
            msg.entrada = n_aula;    /* Numero de aula */
            msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
            
        }

    }
}


/* **************** Alumnos ************************** */

void manejadorAlumno_SIGTERM(int signal){
    printf("Alumno %d - Entrego examen por falta de tiempo\n",getpid());
    exit(EXIT_SUCCESS);
}



void alumno(int msqid){
    Mensaje msg;

    /* Armamos manejador */
    if(signal(SIGTERM, manejadorAlumno_SIGTERM) == SIG_ERR){
        perror("Error al armar el manejador de SIGTERM en alumno");
        exit(EXIT_FAILURE);
    }
    
    srand(getpid());
    sleep(rand() % T_ALUMNO);
    printf("Alumno %d - Intentando entrar en un aula\n",getpid());
    
    /* Enviamos mensaje al gestor */
    msg.id = 3;             /* 3 indica gestor */
    msg.pid = getpid();     /* Pid del alumno */
    msg.entrada = 0;        /* 0 indica llegada */
    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);

    /* Esperamos mensaje de un profesor de entrada */
    msgrcv(msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), getpid(), 0);

    printf("Alumno %d - Comenzando a hacer el examen en silla %d aula %d\n", getpid(), msg.pid + 1, msg.entrada);
    sleep((rand() % T_ALUMNO_EXAMEN) + T_MIN);

    printf("Alumno %d - Finalizado de hacer examen en silla %d aula %d\n", getpid(), msg.pid , msg.entrada);
    
    /* Enviamos mensaje al aula de examen finalizado al profesor de salida */
    msg.id = msg.entrada + 5;   /* Numero de aula + 5 */
    msg.pid = getpid();     /* Pid del alumno */
    msg.entrada = 1;        /* 1 indica entrega */
    msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), 0);
    exit(EXIT_SUCCESS);
}

void actualiza_fichero2(){
    char cmd[60];
    printf("Actualizando fichero...\n");
    switch(fork()){
    case -1:
        perror("Error en el fork");
        exit(EXIT_FAILURE);
    case 0:
        
        sprintf(cmd, "echo New Update >> SIGHUP_%d_lista_proc.txt", getppid());
        system(cmd);
        sprintf(cmd, "ps -ef | grep %d | grep -v grep >> SIGHUP_%d_lista_proc.txt", getppid(), getppid());
        system(cmd);
        exit(EXIT_SUCCESS);
    default:
        wait(NULL);
    }
}

