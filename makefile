CC=g++
CFLAGS=-I/usr/include/boost/
LIBS=-lboost_system -lboost_filesystem

tests: main/bbox.o test/tests.o
	$(CC) $(CFLAGS) main/bbox.o test/tests.o $(LIBS) -o tests

main/bbox.o: main/bbox.hxx main/bbox.cxx algo/bounding.hxx main/types.hxx
	$(CC) $(CFLAGS) -c main/bbox.cxx -o main/bbox.o

test/tests.o: algo/surface.hxx algo/volume.hxx algo/bounding.hxx test/tests.hxx test/tests.cxx parser/parser.hxx main/bbox.hxx main/types.hxx
	$(CC) $(CFLAGS) -c test/tests.cxx -o test/tests.o

clean:
	rm tests main/bbox.o test/tests.o
