CC=g++
FLAGS=-Wall -std=c++14

sns.o: sns.cpp data_structures.o threads.o
	$(CC) $(FLAGS) -o sns threads.o data_structures.o sns.cpp -lpthread

threads.o: threads.cpp threads.hpp
	$(CC) $(FLAGS) -c threads.cpp 

data_structures.o: data_structures.cpp data_structures.hpp
	$(CC) $(FLAGS) -c data_structures.cpp

clean:
	rm -f *.o

distclean: 
	rm -f *.o sns
