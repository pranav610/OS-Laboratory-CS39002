CC=g++
FLAGS=-Wall -std=c++14

shell: shell.o delep.o history.o squashbug.o
	$(CC) $(FLAGS) -o shell shell.o delep.o history.o squashbug.o -lreadline

shell.o: shell.cpp
	$(CC) $(FLAGS) -c shell.cpp -lreadline

delep.o: delep.cpp delep.hpp
	$(CC) $(FLAGS) -c delep.cpp

history.o: history.cpp history.hpp
	$(CC) $(FLAGS) -c history.cpp -lreadline

squashbug.o: squashbug.cpp squashbug.hpp
	$(CC) $(FLAGS) -c squashbug.cpp
