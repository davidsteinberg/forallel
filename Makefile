forallel : forallel.o
	g++ -Wall -I/usr/include/libxml2 -lxml2 -o forallel forallel.o;

forallel.o : forallel.cpp forallel.hpp
	g++ -Wall -c -I/usr/include/libxml2 forallel.cpp

#This will clean up everything via "make clean"
clean :
	rm -f forallel *.o

cleano :
	rm -f *.o