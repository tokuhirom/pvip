CFLAGS=-Wall -fPIC -g
# CFLAGS+= -DYY_DEBUG=1 -DDEBUG_PARSE
# -std=c89
CC=cc

all: pvip qre

qre: libpvip.a src/qre/main.c src/commander.o
	gcc $(CFLAGS) -o qre src/qre/main.c libpvip.a

pvip: libpvip.a src/main.c src/commander.o
	gcc $(CFLAGS) -o pvip src/commander.o src/main.c libpvip.a

libpvip.a: src/gen.pvip.y.o src/pvip.h src/pvip_node.o src/gen.node.o src/pvip_string.o src/qre/qre.o src/qre/gen.qre.g.o
	ar rsv libpvip.a src/gen.pvip.y.o src/pvip_node.o src/gen.node.o src/pvip_string.o src/qre/qre.o src/qre/gen.qre.g.o

src/qre/qre.o: src/qre/qre.c
	$(CC) $(CFLAGS) -c -o src/qre/qre.o src/qre/qre.c

test: pvip t/c_level.t
	prove -lr t

.c.o: src/pvip.h
	$(CC) $(CFLAGS) -c -o $@ $<

t/c_level.t: src/c_level_test.o libpvip.a
	$(CC) $(CFLAGS) -o t/c_level.t src/c_level_test.o libpvip.a

src/main.o: src/pvip.h
src/pvip_node.o: src/pvip.h src/pvip_private.h
src/gen.pvip.y.o: src/pvip.h src/pvip_private.h

src/gen.node.c: build/node.pl src/pvip.h
	perl build/node.pl

3rd/greg/greg:
	cd 3rd/greg/ && $(CC) -g -o greg greg.c compile.c tree.c

src/gen.pvip.y.c: src/pvip.y src/pvip.h src/gen.node.c 3rd/greg/greg
	./3rd/greg/greg -o src/gen.pvip.y.c src/pvip.y

src/qre/gen.qre.g.o: src/qre/gen.qre.g.c

src/qre/gen.qre.g.c: src/qre/qre.g 3rd/greg/greg
	./3rd/greg/greg -o src/qre/gen.qre.g.c src/qre/qre.g

clean:
	rm -f src/*.o src/gen.* pvip libpvip.a

.PHONY: all test

