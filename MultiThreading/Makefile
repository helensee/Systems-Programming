COMPILER = gcc
CCFLAGS = -Wall

all: order

data.o: data.c data.h
	$(COMPILER) $(CCFLAGS) -c -lpthread data.c

main.o: main.c data.h
	$(COMPILER) $(CCFLAGS) -c -lpthread main.c

liborder.a: data.o
	ar rvs liborder.a data.o

order: main.o liborder.a
	$(COMPILER) $(CCFLAGS) -g -o order -lpthread main.o liborder.a

clean:
	rm -f order
	rm -f liborder.a
	rm -f *.o
