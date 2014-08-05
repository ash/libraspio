all: libraspio.a

raspio.o:
	gcc -c -Iinclude/ src/raspio.c -oraspio.o

libraspio.a: raspio.o
	ar cq libraspio.a raspio.o

test: libraspio.a test.out

test.out:
	gcc -Iinclude/ t/test.c libraspio.a -o test.out
	./test.out

install:
	cp include/raspio.h /usr/local/include/
	cp libraspio.a /usr/local/lib/

clean:
	rm -f test.out *.o *.a

