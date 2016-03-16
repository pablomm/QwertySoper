CC = gcc -pedantic
CFLAGS = -Wall
EXE = ejercicio3a ejercicio3b ejercicio4 ejercicio6 ejercicio8 ejercicio10
LIBS = -lm -lrt -lpthread

all : $(EXE)
	rm -f *.o
.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<


