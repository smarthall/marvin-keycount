
.PHONY: all clean
all:  marvinlog

marvinlog: x11keycount.o main.o
	gcc -Wall -lX11 main.o x11keycount.o -o marvinlog

main.o: main.c
	gcc -Wall -c -std=c99 main.c -o main.o

x11keycount.o: x11keycount.c x11keycount.h
	gcc -Wall -c -std=c99 x11keycount.c -o x11keycount.o

clean:
	-rm -f marvinlog x11keycount.o
