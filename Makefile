main: array.o
		cc -o main array.o

array.o: array.c array.h
		cc -c array.c

clean: 
		rm main array.o