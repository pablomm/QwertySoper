
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#define MAX 10
#define NITER 50



void manejador_SIGALRM(int signal);
int main(void){
    pid_t childpid = -1;
    FILE * pfile = NULL;
    int i=0;
    int flag=0;
    int childexit=0;
    sigset_t mask;

	const char fichero[]= "fichero.txt";
    char cadena[][MAX] = {"EL", "PROCESO", "ESCRIBE", "EN", "UN", "FICHERO",
"HASTA", "QUE", "LEE", "LA", "CADENA", "FIN"};
    char palabra[MAX];
    char buffer[MAX];


  /*Mascara para esperar a la alarma*/
    if(sigfillset(&mask)==-1){
        perror("Error en la mascara");
        exit(EXIT_FAILURE);
    }
    if(sigdelset(&mask,SIGALRM)==-1){
        perror("Error en la mascara");
        exit(EXIT_FAILURE);
    }        

    /*Armamos un manejador para la alarma*/
    if(signal(SIGALRM, manejador_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }


    for (i=0;i<NITER;){    
        /*Lanzamiento del proceso hijo*/
        printf("Lanzamiento de un hijo\n");
        childpid=fork();
	    if(childpid==-1){
            perror("Error en la creacion del porceso hijo");
            exit(EXIT_FAILURE);
        }

	    /*Linea de ejecucion del padre*/
        if(childpid>0){
             if(alarm(3)){
                    perror("Ya habia una alarma creada");
                    exit(EXIT_FAILURE);
                }
                /*Esperamos hasta que se reciba la alarma*/
                sigsuspend(&mask);
            flag=0;
            for(;i<NITER && flag == 0;i++){
                pfile=fopen(fichero, "r");
                if(pfile==NULL){
                    perror("No se pudo abrir el fichero");
                    exit(EXIT_FAILURE);
                }
                fgets(buffer, MAX, pfile);
                printf("He leido la palabra %s\n", buffer);
                fflush(NULL);
                fclose(pfile);
                if(strcmp("FIN", buffer)==-32){
                    if(wait()==EXIT_FAILURE){
                        exit(EXIT_FAILURE);
                    }
                    flag=1;
                } else {
                    /*Se genera una alarma cada 5 segundos*/
                    if(alarm(5)){
                        perror("Ya habia una alarma creada");
                        exit(EXIT_FAILURE);
                    }
                    /*Esperamos hasta que se reciba la alarma*/
                    sigsuspend(&mask);
                }
         
            }
		
        /*Proceso hijo*/
	    }
        if(childpid==0){
          
            /*Seed*/
		    srand(getpid());
          

            /*Bucle principal*/
            while(1){
                
                pfile=fopen(fichero, "w+");
                if(pfile==NULL){
                    perror("No se pudo abrir el fichero");
                    exit(EXIT_FAILURE);
                }
                strcpy(palabra,cadena[rand()%12]);
                fprintf(pfile,"%s ", palabra);
                printf("Soy hijo, y he escrito la palabra %s\n", palabra);
                fclose(pfile);
                if(strcmp("FIN", palabra)==0){
                    printf("Soy el hijo, y me acabo\n");
                    exit(EXIT_SUCCESS);
                    
                }

                /*Se genera una alarma cada 5 segundos*/
                if(alarm(5)){
                    perror("Ya habia una alarma creada");
                    exit(EXIT_FAILURE);
                }
                /*Esperamos hasta que se reciba la alarma*/
                sigsuspend(&mask);

            } 
	    }

    }
    if(flag==0){
        printf("Se han hecho %d lecturas\n",i);
        kill(childpid, SIGKILL);
        wait();
    }
    exit(EXIT_SUCCESS);

}

void manejador_SIGALRM(sig){
    signal(sig, SIG_IGN);
    /*Volvemos a armar el manejador*/
    if(signal(SIGALRM, manejador_SIGALRM)==SIG_ERR){
        perror("Error al armar el manejador");
        exit(EXIT_FAILURE);
    }
}


