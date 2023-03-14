CC=g++
FLAGS=-Wall -std=c++14

main: main.cpp producer consumer
	$(CC) $(FLAGS) -o main main.cpp

producer: producer.cpp
	$(CC) $(FLAGS) -o producer producer.cpp

consumer: consumer.cpp
	$(CC) $(FLAGS) -o consumer consumer.cpp

clean: 
	rm -f main producer consumer
	rm -f ?.txt