client-objs = client.o processes.o networking.o
client-libs = -l ncurses -l cdk
server-objs = server.o array.o log.o

all: client server

client-debug:
	gcc -g -c client/client.c client/processes.h
	gcc -g -c client/processes.c include/protocol.h client/processes.h client/networking.h
	gcc -g -c client/networking.c client/networking.h include/protocol.h
	gcc -o client_debug $(client-objs) $(client-libs)

client: $(client-objs)
	gcc -o client_run $(client-objs) $(client-libs)

server: $(server-objs)
	gcc -o server_run $(server-objs)

client.o: client/client.c client/processes.h
	gcc -c client/client.c client/processes.h

processes.o: client/processes.c include/protocol.h client/processes.h client/networking.h
	gcc -c client/processes.c include/protocol.h client/processes.h client/networking.h

networking.o: client/networking.c client/networking.h include/protocol.h
	gcc -c client/networking.c client/networking.h include/protocol.h

server.o: server/server.c server/server.h include/protocol.h
	gcc -c server/server.c include/protocol.h server/server.h

array.o: server/array.c server/array.h include/protocol.h
	gcc -c server/array.c server/array.h include/protocol.h

log.o: server/log.c server/log.h include/protocol.h
	gcc -c server/log.c server/log.h include/protocol.h

clean:
	rm -f *~
	rm -f *.o
	rm -rf data
	rm -f client_run server_run client_debug test-client test-serv

build-test:
	gcc -o test-client test/client_test.c
	gcc -o test-serv test/serv.c

