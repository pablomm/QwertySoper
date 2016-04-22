CC = gcc -pedantic 
CFLAGS = -w
EXE =  cadena_montaje
LDFLAGS = -lpthread

all : $(EXE)
	rm -f *.o
	@echo "#---------------------------"
	@echo " Pablo Marcos"
	@echo " David Nevado"
	@echo " Pareja 10"
	@echo "#---------------------------"
.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o semaforos.o
	$(CC) $(CFLAGS) -o $@ $@.o semaforos.o $(LDFLAGS)

%.o : %.c semaforos.h
	$(CC) $(CFLAGS) -c $<
