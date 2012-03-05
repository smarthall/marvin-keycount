CC=gcc
CFLAGS=-g -Wall -c -std=c99 -pg
LDFLAGS=-Wall -lX11 -pg

.PHONY: all clean install
all:  marvinlog

marvinlog: x11keycount.o main.o tcpserver.o
	$(CC) $(LDFLAGS) main.o x11keycount.o tcpserver.o -o marvinlog

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o

x11keycount.o: x11keycount.c x11keycount.h
	$(CC) $(CFLAGS) x11keycount.c -o x11keycount.o

tcpserver.o: tcpserver.c tcpserver.h
	$(CC) $(CFLAGS) tcpserver.c -o tcpserver.o

install: marvinlog
	install -d $(DESTDIR)/usr/bin
	install -m 0755 marvinlog $(DESTDIR)/usr/bin/marvinlog

clean:
	-rm -f marvinlog *.o
