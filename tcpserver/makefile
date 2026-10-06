CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
DEPS= tcpserver.h
OBJ = tcpserver.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tcpserver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
