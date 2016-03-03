#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define _POSIX_C_SOURCE 199309L

int aleat(){
	struct timespec time;
	/*Generate seed*/
	clock_gettime(CLOCK_REALTIME, &time);
	srand(time.tv_nsec);
	return rand();

}

void main (void) {
	int i=0;
	pid_t pid=-1;
	pid_t father=-1;

	
	father=getpid();

	for (i=0; i<100 && father==getpid(); i++){
		
		pid=fork();
		if(pid==-1){
			perror("Error al crear hijo\n");
			exit(EXIT_FAILURE);
		} else if(pid==0){
			printf("Numero aleatorio: %d\n", aleat());
		}

	}
	for (i=0; i<100 && father==getpid(); i++){
		wait();	
	}
	
	exit(EXIT_SUCCESS);

}
