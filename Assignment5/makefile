main: main.cpp guest.o cleaner.o data_structures.hpp 
	g++ -o main main.cpp guest.o cleaner.o -lpthread -lrt

guest.o: guest.cpp guest.hpp data_structures.hpp
	g++ -c guest.cpp -lpthread -lrt

cleaner.o: cleaner.cpp cleaner.hpp data_structures.hpp
	g++ -c cleaner.cpp -lpthread -lrt

clean:
	rm *.o main