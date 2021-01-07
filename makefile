river: main.o river.o
	g++ -Wall -g main.o river.o -o river

main.o: main.cpp river.h
	g++ -Wall -g -c main.cpp

river.o: river.cpp river.h
	g++ -Wall -g -c river.cpp