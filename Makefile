CC = gcc -ansi -pedantic
CFLAGS = -Wall
EXE = ejercicio4a ejercicio4b ejercicio5a ejercicio5b ejercicio6 ejercicio8a ejercicio8b ejercicio9
LIBS = -lm -lrt

all : $(EXE)
	rm -f *.o
.PHONY : clean
clean :
	rm -f *.o core $(EXE)

$(EXE) : % : %.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<


