CPPC=g++
CC=gcc
CFLAGS=-I. -pthread -lm -std=c++14
TESTFLAGS=-lgtest -Wall
DEPS = fork-tree.h
OBJ = fork-tree.o
TREETESTOBJ = fork-tree-test.o
TESTOBJ = fork-exercise-test.o
EXOBJ = fork-exercise.o
BIN = ftest extest exercises exdone

%.o: %.cpp $(DEPS)
	$(CPPC) -c -o $@ $< $(CFLAGS) $(TESTFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(TESTFLAGS)

ftest: $(OBJ) $(TREETESTOBJ)
	$(CPPC) -o $@ $^ $(CFLAGS) $(TESTFLAGS)

extest: $(OBJ) $(TESTOBJ)
	$(CPPC) -o $@ $^ $(CFLAGS) $(TESTFLAGS)

exercises: $(EXOBJ)
	$(CC) -o $@ $^ $(CFLAGS)


test: exercises extest
	./exercises > test.out
	./extest

clean:
	rm $(BIN) *.o *.out *.log 
