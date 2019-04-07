CC = gcc
FILENAME = prog

all:
	${CC} -Wall -o ${FILENAME} port_fwd.c
	
port_forwarder: port_forwarder.c
	${GCC} -Wall -o ${FILENAME} port_fwd.c
	
clean:
	rm prog