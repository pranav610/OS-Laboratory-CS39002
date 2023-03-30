main: main.cpp guest.o cleaner.o data_structures.o data_structures.hpp 
	g++ -Wall -o main main.cpp guest.o cleaner.o data_structures.o -lpthread -lrt

guest.o: guest.cpp threads.hpp data_structures.o
	g++ -Wall -c guest.cpp -lpthread -lrt

cleaner.o: cleaner.cpp threads.hpp data_structures.o
	g++ -Wall -c cleaner.cpp -lpthread -lrt

data_structures.o: data_structures.hpp
	g++ -Wall -c data_structures.cpp -lpthread -lrt

clean:
	rm *.o

distclean:
	rm *.o main log.txt
