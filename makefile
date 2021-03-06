CC=g++
CFLAGS=-I/usr/include/boost/
LIBS=-lboost_program_options -lboost_system -lboost_filesystem

all: demo tests

demo: main/bbox.o main/demo.o 
	$(CC) $(CFLAGS) main/bbox.o main/demo.o $(LIBS) -o demo

tests: main/bbox.o test/tests.o
	$(CC) $(CFLAGS) main/bbox.o test/tests.o $(LIBS) -o tests

main/demo.o: main/demo.hxx main/demo.cxx main/bbox.hxx algo/surface.hxx algo/volume.hxx algo/bounding.hxx main/types.hxx
	$(CC) $(CFLAGS) -c main/demo.cxx -o main/demo.o

main/bbox.o: main/bbox.hxx main/bbox.cxx algo/bounding.hxx main/types.hxx
	$(CC) $(CFLAGS) -c main/bbox.cxx -o main/bbox.o

test/tests.o: algo/surface.hxx algo/volume.hxx algo/bounding.hxx test/tests.hxx test/tests.cxx parser/parser.hxx main/bbox.hxx main/types.hxx
	$(CC) $(CFLAGS) -c test/tests.cxx -o test/tests.o

clean:
	rm -f tests demo main/bbox.o test/tests.o main/demo.o
