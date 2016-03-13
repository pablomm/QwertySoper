#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SECS 5
#define MAX_READ 50
#define MAX_TAM 10
#define N_WORDS 13

int main(void){
    char frase[][MAX_TAM]={"EL","PROCESO","A","ESCRIBE","EN","UN","FICHERO","HASTA","QUE","LEE","LA","PALABRA","FIN"};
    char buff[MAX_TAM];
    char *palabra;
    int i=0,flag;
    pid_t pid;
    FILE *fp;

    /* Tratamiento de la sennal de alarma */
    if (signal(SIGALRM, SIG_IGN) == SIG_ERR){
        perror("Error en la captura");
        exit (EXIT_FAILURE);
    }
    /* Abrimos el archivo en modo lecturo para B */
    fp = fopen("archivo.txt","r");
    if(!fp){
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }
    
    while(1){ /* Bucle para relanzar proceso A */
        flag=0;
        switch(pid=fork()){
            case -1: /* Caso de Error */
                perror("Error en el fork");
                exit(EXIT_FAILURE);

            case 0: /* Proceso A */
                srand(getpid());
                fclose(fp);
                
                while(1){ /* Bucle para escribir */

                    fp = fopen("archivo.txt","a");
                    if(!fp){
                        perror("Error abriendo el archivo");
                        exit(EXIT_FAILURE);
                    }

                    if(alarm(SECS))
                        pause();
       
                    palabra = frase[rand()%N_WORDS];
                    printf("Proceso A - Escribiendo: %s\n",palabra);      
                    fprintf(fp,"%s ",palabra);

                    fclose(fp);

                    if(!strcmp(palabra,"FIN")){
                        printf("Proceso A - Finalizando\n");
                        exit(EXIT_SUCCESS);
                    }       
                }
            default: /* Proceso B */
                for( ;i<MAX_READ;i++){
                    if(alarm(SECS))
                        pause();
                    fscanf(fp,"%9s",buff);
                    printf("Proceso B - Leyendo: %s\n",buff);
                    if(!strcmp(buff,"FIN")){
                        flag=1;
                        break;
                    }
                }
        }
        if(flag==0)break;
    } 
    /* Mandamos señal KILL antes de finalizar */
    kill(pid,9);
    exit(EXIT_SUCCESS);
}


