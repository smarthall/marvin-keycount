CC=gcc
CFLAGS=-Wall -c -std=c99
LDFLAGS=-Wall -lX11

.PHONY: all clean
all:  marvinlog

marvinlog: x11keycount.o main.o tcpserver.o
	$(CC) $(LDFLAGS) main.o x11keycount.o -o marvinlog

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o

x11keycount.o: x11keycount.c x11keycount.h
	$(CC) $(CFLAGS) x11keycount.c -o x11keycount.o

tcpserver.o: tcpserver.c tcpserver.h
	$(CC) $(CFLAGS) tcpserver.c -o tcpserver.o

clean:
	-rm -f marvinlog *.o
