run: all
	./run

all: server.o client.o
	gcc -o run server.o client.o

server.o: server.c headers.h
	gcc -c server.c

client.o: client.c headers.h
	gcc -c client.c

clean:
	rm -rf *~
	rm -rf *.o
	rm -rf run