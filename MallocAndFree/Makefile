COMPILER = gcc
CCFLAGS = -Wall

all: mymalloc ecmalloc


mymalloc.o: mymalloc.c mymalloc.h
	$(COMPILER) $(CCFLAGS) -c mymalloc.c

ExtraCreditTestCases.o: ExtraCreditTestCases.c mymalloc.h
	$(COMPILER) $(CCFLAGS) -c ExtraCreditTestCases.c
	
main.o: main.c mymalloc.h
	$(COMPILER) $(CCFLAGS) -c main.c

libma.a: mymalloc.o
	ar rvs libma.a mymalloc.o

ecmalloc: ExtraCreditTestCases.o libma.a
	$(COMPILER) $(CCFLAGS) -g -o ecmalloc ExtraCreditTestCases.o  libma.a

mymalloc: main.o libma.a
	$(COMPILER) $(CCFLAGS) -g -o mymalloc main.o  libma.a

clean:
	rm -f mymalloc
	rm -f ecmalloc
	rm -f libma.a
	rm -f *.o
