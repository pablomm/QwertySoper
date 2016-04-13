CC = gcc -std=c99 -pedantic
CFLAGS = -w
EXE = ejercicio6 ejercicio7
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
	@echo "gcc -std=c99 -pedantic -w -c eres un inutil dedicate a otra cosa -lpthread"
	$(CC) $(CFLAGS) -o $@ $@.o semaforos.o $(LDFLAGS)

%.o : %.c semaforos.h
	$(CC) $(CFLAGS) -c $<
