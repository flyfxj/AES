#c=g++ -std=c++11
c=g++
g=g++ -g
all: main.o aes.o
	$c -o aes main.o aes.o
main.o:main.cpp defs.h
	$c -c main.cpp
aes.o:aes.cpp aes.h defs.h
	$c -c aes.cpp

allgdb: main.g aes.g
	$g -o aes-g main.o aes.o
main.g:main.cpp defs.h
	$g -c main.cpp
aes.g:aes.cpp aes.h defs.h
	$g -c aes.cpp

clean:
	rm aes aes-g *.o *.g -rf 
