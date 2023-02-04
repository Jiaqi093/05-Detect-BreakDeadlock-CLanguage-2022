detect: detect.o utility.o
	gcc detect.o utility.o -o detect

detect.o: detect.c utility.h
	gcc detect.c -c -Wall -g -o detect.o

utility.o: utility.c utility.h 
	gcc utility.c -c -Wall -g -o utility.o

clean:
	rm -f detect *.o