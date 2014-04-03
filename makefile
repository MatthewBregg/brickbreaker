#Compiler
CC = g++
#Cflags
CFLAGS =  -Wall -std=c++0x -I.
#Libraries
LIB =  -pthread -lncurses -lsfml-audio
all: main


main: main.o
	$(CC) $(CFLAGS) main.o -o main $(LIB)


main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp $(LIB)  


clean: *.o
	rm *.o
