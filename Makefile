CC = gcc
FILENAME = FinalProj

all:
	${CC} -Wall -o ${FILENAME} port_fwd.c

debug:
	${CC} -ggdb -Wall -o ${FILENAME} port_fwd.c

port_forwarder: port_forwarder.c
	${CC} -Wall -o ${FILENAME} port_fwd.c

clean:
	rm ${FILENAME}
