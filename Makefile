arff: main.o arff.o log.o 
	gcc main.o arff.o log.o -o arff
	
main.o: main.c arff.h log.h
	gcc -c main.c -o main.o

arff.o: arff.c arff.h
	gcc -c arff.c -o arff.o

log.o:	log.c log.h arff.h
	gcc -c log.c -o log.o
	
clean:
	rm  -f *.o
	
purge:	clean
	rm -f arff