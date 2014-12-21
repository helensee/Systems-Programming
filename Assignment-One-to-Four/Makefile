COMPILER = gcc
CCFLAGS = -Wall

all: index search


sorted-list.o: sorted-list.c sorted-list.h
	$(COMPILER) $(CCFLAGS) -c sorted-list.c

sorted-token.o: sorted-token.c sorted-token.h
	$(COMPILER) $(CCFLAGS) -c sorted-token.c

tokenizer.o: tokenizer.c tokenizer.h
	$(COMPILER) $(CCFLAGS) -c tokenizer.c

main.o: main.c sorted-list.h sorted-token.h tokenizer.h
	$(COMPILER) $(CCFLAGS) -c main.c

libsl.a: sorted-list.o sorted-token.o tokenizer.o
	ar rvs libsl.a sorted-list.o sorted-token.o tokenizer.o

index: main.o libsl.a
	$(COMPILER) $(CCFLAGS) -g -o index main.o libsl.a

search.o: search.c
	$(COMPILER) $(CCFLAGS) -c search.c

search: search.o
	$(COMPILER) $(CCFLAGS) -g -o search search.o
 
clean:
	rm -f index
	rm -f libsl.a
	rm -f *.o
	rm -f search
