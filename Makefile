CC=g++
CFLAGS=-g -Wall -std=c++14 -I.
TARGET=kaleidoscope
OBJ=parser.o handler.o main.o

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -rf *.o kaleidoscope