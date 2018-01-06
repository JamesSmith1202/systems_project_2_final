all:
	make client server

run:
	gnome-terminal -e ./server_run
	./client_run

client-debug:
	gcc -g -c client/client.c client/processes.h
	gcc -g -c client/processes.c include/protocol.h client/processes.h client/networking.h
	gcc -g -c client/networking.c client/networking.h
	gcc -o client_run client.o processes.o networking.o -l ncurses -l cdk

client: client.o processes.o networking.o
	gcc -o client_run client.o processes.o networking.o -l ncurses -l cdk

server: server.o
	gcc -o server_run server.o

server.o: server/server.c include/protocol.h
	gcc -c server/server.c include/protocol.h

client.o: client/client.c client/processes.h
	gcc -c client/client.c client/processes.h

processes.o: client/processes.c include/protocol.h client/processes.h client/networking.h
	gcc -c client/processes.c include/protocol.h client/processes.h client/networking.h

networking.o: client/networking.c client/networking.h
	gcc -c client/networking.c client/networking.h

clean:
	rm -f *~
	rm -f *.o
	rm -f client_run server_run prototype

client_proto: proj_proto.c
	gcc -o prototype proj_proto.c -l ncurses -l cdk

