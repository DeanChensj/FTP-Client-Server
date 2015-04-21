CC = gcc
CFLAGS = -g -I./
SRCCLIENT = .
SRCSERVER = .
BINCLIENT = bin/client
BINSERVER = bin/server
OBJCLIENT = obj
OBJSERVER = obj
OBJECTSCLIENT =  ${OBJCLIENT}/function.o ${OBJCLIENT}/C.o
OBJECTSSERVER =  ${OBJSERVER}/function.o ${OBJSERVER}/S.o 
EXECUTABLECLIENT = ${BINCLIENT}/C
EXECUTABLESERVER = ${BINSERVER}/S

all:	clean client server

client:	${OBJECTSCLIENT}
	${CC} ${CFLAGS} $^ -o ${EXECUTABLECLIENT}

server:	${OBJECTSSERVER}
	${CC} ${CFLAGS} $^ -o ${EXECUTABLESERVER}

${OBJCLIENT}/%.o:	${SRCCLIENT}/%.c
	${CC} ${CFLAGS} -c $< -o $@

${OBJSERVER}/%.o:	${SRCSERVER}/%.c
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -f ${OBJCLIENT}/*.o
	rm -f ${OBJSERVER}/*.o
	rm -f ${BINCLIENT}/*.out
	rm -f ${BINSERVER}/*.out
